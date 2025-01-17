#pragma once

#include "autogen/utils/file_utils.hpp"

#if CPPAD_CG_SYSTEM_WIN
#include <windows.h>

namespace {
static std::string GetLastErrorAsString() {
  // https://stackoverflow.com/a/17387176
  // Get the error message ID, if any.
  DWORD errorMessageID = ::GetLastError();
  if (errorMessageID == 0) {
    return std::string();  // No error message has been recorded
  }

  LPSTR messageBuffer = nullptr;

  // Ask Win32 to give us the string version of that message ID.
  // The parameters we pass in, tell Win32 to create the buffer that holds the
  // message for us (because we don't yet know how long the message string will
  // be).
  size_t size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPSTR)&messageBuffer, 0, NULL);

  // Copy the error message into a std::string.
  std::string message(messageBuffer, size);

  // Free the Win32's string's buffer.
  LocalFree(messageBuffer);

  return message;
}
}  // namespace
#endif

#include "cuda_library_processor.hpp"
#include "cuda_model.hpp"

namespace autogen {
template <typename Scalar>
class CudaLibrary {
 protected:
  using ModelInfoFunctionPtr = void (*)(char const *const **names, int *count);

  void *lib_handle_{nullptr};

  std::map<std::string, CudaModel<Scalar>> models_;

 public:
  /**
   * Opens the dynamic library with the given basename (filename without
   * extension), and loads the CUDA models.
   */
#if CPPAD_CG_SYSTEM_WIN
  CudaLibrary(const std::string &library_basename, std::string path = "") {
    path += library_basename + ".dll";
    std::string abs_path;
    bool found = autogen::FileUtils::find_file(path, abs_path);
    assert(found);
    lib_handle_ = LoadLibrary(abs_path.c_str());
    if (lib_handle_ == nullptr) {
      throw std::runtime_error("Failed to dynamically load CUDA library '" +
                               library_basename + "' (error code " +
                               std::to_string(GetLastError()) +
                               "): " + GetLastErrorAsString());
    }
#else
  CudaLibrary(const std::string &library_basename, std::string path = "",
              int dlOpenMode = RTLD_NOW) {
    path += library_basename + ".so";
    std::string abs_path;
    bool found = autogen::FileUtils::find_file(path, abs_path);
    assert(found);
    lib_handle_ = dlopen(abs_path.c_str(), dlOpenMode);
    // _dynLibHandle = dlmopen(LM_ID_NEWLM, path.c_str(), RTLD_NOW);
    if (lib_handle_ == nullptr) {
      throw std::runtime_error("Failed to dynamically load CUDA library '" +
                               library_basename +
                               "': " + std::string(dlerror()));
    }
#endif
    auto model_info_fun =
        CudaFunction<Scalar>::template load_function<ModelInfoFunctionPtr>(
            "model_info", lib_handle_);
    const char *const *names;
    int count;
    model_info_fun(&names, &count);
    std::cout << "Found " << count << " model";
    if (count != 1) std::cout << "s";
    std::cout << ": ";
    for (int i = 0; i < count; ++i) {
      std::cout << names[i];
      if (i < count - 1) std::cout << ", ";
      models_.emplace(std::make_pair(std::string(names[i]),
                                     CudaModel<Scalar>(names[i], lib_handle_)));
    }
    std::cout << std::endl;
  }

  virtual ~CudaLibrary() {
    // clear models, which will deallocate GPU memory
    // before the library is unloaded
    models_.clear();
#if CPPAD_CG_SYSTEM_WIN
    FreeLibrary((HMODULE)lib_handle_);
#else
    dlclose(lib_handle_);
#endif
    lib_handle_ = nullptr;
  }

  const CudaModel<Scalar> &get_model(const std::string &model_name) const {
    return models_.at(model_name);
  }
  bool has_model(const std::string &model_name) const {
    return models_.find(model_name) != models_.end();
  }

  std::vector<std::string> model_names() const {
    std::vector<std::string> names(models_.size());
    for (const auto &[key, value] : models_) {
      names.push_back(key);
    }
    return names;
  }
};
}  // namespace autogen