from typing import Callable
from _autogen import *
from enum import Enum

# keeps track of the current autogen mode and other meta information
__AUTOGEN_SCOPE__ = None


class Mode(Enum):
    NUMERICAL = 1
    CPPAD = 2
    CPU = 3
    GPU = 4


class Scope:
    def __init__(self, mode: Mode) -> None:
        self.mode = mode


def scalar_type():
    global __AUTOGEN_SCOPE__
    if __AUTOGEN_SCOPE__ is None or __AUTOGEN_SCOPE__.mode == Mode.NUMERICAL:
        return float
    if __AUTOGEN_SCOPE__.mode == Mode.CPPAD:
        return ADScalar
    # return ADCGScalar


def scalar(x):
    global __AUTOGEN_SCOPE__
    if __AUTOGEN_SCOPE__ is None or __AUTOGEN_SCOPE__.mode == Mode.NUMERICAL:
        return float(x)
    if __AUTOGEN_SCOPE__.mode == Mode.CPPAD:
        return ADScalar(x)
    # return ADCGScalar(x)


def vector_type():
    global __AUTOGEN_SCOPE__
    if __AUTOGEN_SCOPE__ is None or __AUTOGEN_SCOPE__.mode == Mode.NUMERICAL:
        return list
    if __AUTOGEN_SCOPE__.mode == Mode.CPPAD:
        return ADVector
    return ADCGVector


def trace(fun, xs, mode: Mode = Mode.CPPAD):
    global __AUTOGEN_SCOPE__
    if __AUTOGEN_SCOPE__ is None:
        __AUTOGEN_SCOPE__ = Scope(mode)
    else:
        __AUTOGEN_SCOPE__.mode = mode

    # Scalar = scalar_type()
    # Vector = vector_type()
    Scalar = ADCGScalarPtr
    Vector = ADCGPtrVector

    print('before creating ad_x')
    converted = Scalar(xs[0])
    print('converted', converted)

    # test = ADCGPtrVector([converted])

    ad_x = Vector([Scalar(x) for x in xs])

    independent(ad_x)
    # for i in range(len(xs)):
    #     xs[i] = ad_x[i]
    # ys = fun(xs)
    ys = fun(ad_x)
    ad_y = Vector(list(ys))
    if mode == Mode.NUMERICAL:
        raise NotImplementedError("finite diff functor not yet implemented")
    if mode == Mode.CPPAD:
        return ADFun(ad_x, ad_y)
    # return ADCGFun(ad_x, ad_y)
    return ADCGPtrFun(ad_x, ad_y)

class Generated:
    def __init__(self, function: Callable[[list], list], name: str,
                 mode: Mode = Mode.CPPAD):
        self.function = function
        self.name = name
        self.mode = mode
        self.__global_input_dim = 0
        self.__local_input_dim = -1
        self.__output_dim = -1
        self.__is_compiled = False

    def __call__(self, x: list) -> list:
        return self.function(x)

    def jacobian(self, x: list) -> list:
        pass


    @property
    def mode(self):
        return self.mode

    @mode.setter
    def mode(self, m):
        if m == self.mode:
            return
        print("Setting mode to", m)
        self.mode = m

    @property
    def input_dim(self):
        return self.__global_input_dim + self.__local_input_dim

    @property
    def output_dim(self):
        return self.__output_dim

    def discard_library(self):
        pass

    @property
    def is_compiled(self):
        return self.__is_compiled



# def trace(fun, xs) -> ADFun:
#     ad_x = ADVector([ADScalar(x) for x in xs])
#     independent(ad_x)
#     for i in range(len(xs)):
#         xs[i] = ad_x[i]
#     ys = fun(xs)
#     ad_y = ADVector(list(ys))
#     f = ADFun(ad_x, ad_y)
#     return f


# def trace_cg(fun, xs) -> ADCGFun:
#     ad_x = CGVector([ADCGScalar(x) for x in xs])
#     independent(ad_x)
#     for i in range(len(xs)):
#         xs[i] = ad_x[i]
#     ys = fun(xs)
#     ad_y = CGVector(list(ys))
#     f = ADCGFun(ad_x, ad_y)
#     return f
