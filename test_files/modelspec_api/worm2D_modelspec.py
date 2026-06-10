import modelspec
from modelspec import field, instance_of, optional
from modelspec.base_types import Base
from typing import Any


@modelspec.define
class FloatParameter(Base):
    """
    A parameter containing a float

    Args:
        value: The float value of the parameter
        message: An optional string message describing the parameter
    """

    value: float = field(validator=instance_of(float))
    message: str = field(default=None, validator=optional(instance_of(str)))


def convert2floatparam(x: Any) -> FloatParameter:
    """
    Convert a value (float, int, etc.) to FloatParameter if not None
    """
    if isinstance(x, FloatParameter):
        return x
    print("convert2floatparam {} (type: {})".format(x, type(x)))
    try:
        if x is not None:
            return FloatParameter(value=float(x), message=None)
        else:
            return None
    except Exception as e:
        print("Error converting: {} to FloatParameter: {}".format(x, e))
        raise e


@modelspec.define
class Body_(Base):
    """
    A worm body (TODO)...

    Args:
        C_agar_par_total: Total tangential drag coefficient for agar in kg/s
        C_agar_perp_total: Total rod normal drag coefficient in agar in kg/s
        C_water_par_total: Total rod tangential drag coefficient for water in kg/s

    """

    C_agar_par_total: FloatParameter = field(
        validator=instance_of(FloatParameter), converter=convert2floatparam
    )
    C_agar_perp_total: FloatParameter = field(
        validator=instance_of(FloatParameter), converter=convert2floatparam
    )
    C_water_par_total: FloatParameter = field(
        default=FloatParameter(message="Default value", value=3.3e-06),
        validator=optional(instance_of(FloatParameter)),
    )

    # default=FloatParameter(message="Default value", value=123)


@modelspec.define
class Worm2D(Base):
    """
    A worm (TODO)...

    Args:
        Body: The worm body
    """

    Body: Body_ = field(validator=instance_of(Body_))


# main method

if __name__ == "__main__":
    c_agar_par_total = FloatParameter(message="This is just a dummy value", value=0.001)

    c_agar_perp_total = 2  # will be converted to FloatParameter via converter

    body = Body_(C_agar_par_total=c_agar_par_total, C_agar_perp_total=c_agar_perp_total)

    worm = Worm2D(Body=body)

    print(worm)

    print(worm.Body.C_agar_par_total.value)
    print(worm.Body.C_water_par_total.value)  # this will be the default value

    print("------")

    # serialize to JSON
    json_str = worm.to_json()
    print("Serialized to JSON:")
    print(json_str)

    worm.to_json_file("worm2d_example.json")
    worm.to_yaml_file("worm2d_example.yaml")

    worm_md = worm.generate_documentation(format="markdown")

    with open("worm2d_spec.md", "w") as d:
        d.write(worm_md)
