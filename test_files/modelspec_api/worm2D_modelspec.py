import modelspec
from modelspec import field, instance_of, optional
from modelspec.base_types import Base
from typing import Any


# Creating an API for generating/loading/editing Worm2D JSON files using modelspec: https://github.com/ModECI/modelspec


def convert2float(x: Any) -> float:
    print("convert2float {} ({})".format(x, type(x)))
    """Convert to float if not None"""
    if x is not None:
        return float(x)
    else:
        return None


def convert2int(x: Any) -> int:
    print("convert2int {} ({})".format(x, type(x)))
    """Convert to int if not None"""
    if x is not None:
        return int(x)
    else:
        return None


@modelspec.define
class StandardParameter(Base):
    """
    ....

    Args:
        ...
    """

    message: str = field(default="Undefined", validator=instance_of(str))

    value: float = field(
        default=None, validator=instance_of(float), converter=convert2float
    )


@modelspec.define
class Body_(Base):
    """
    ....

    Args:
        ...
    """

    C_agar_par_total: StandardParameter = field(
        default=None, validator=optional(instance_of(StandardParameter))
    )
    C_agar_perp_total: StandardParameter = field(
        default=None, validator=optional(instance_of(StandardParameter))
    )


@modelspec.define
class Worm2D(Base):
    """
    ....

    Args:
        ...
    """

    Body: Body_ = field(default=None, validator=optional(instance_of(Body_)))


# main method
if __name__ == "__main__":
    # create a new model
    body = Body_()
    body.C_agar_par_total = StandardParameter(
        message="Total tangential drag coefficient for agar in kg/s", value=0.0032
    )
    body.C_agar_perp_total = StandardParameter(value=0.128)

    worm2d = Worm2D()
    worm2d.Body = body

    # print the model
    print(worm2d)

    filename = "worm2d_test.json"
    worm2d.to_json_file(filename)
    print("Saved to {}".format(filename))
