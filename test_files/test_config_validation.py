import copy
import math
import unittest

import helper_funcs as hf


def minimal_config():
    return {
        "nervous_system": {
            "cell_names": {"value": ["A_0"]},
            "cells": {
                "A_0": {
                    "bias": {"value": 0.0},
                    "tau": {"value": 1.0},
                    "gain": {"value": 1.0},
                    "state": {"value": 0.0},
                }
            },
            "chemical_conns": {"value": []},
            "electrical_conns": {"value": []},
        },
        "evolvable_ranges": {},
    }


class ConfigurationValidationTests(unittest.TestCase):
    def assert_validation_error(self, config, expected_text):
        with self.assertRaisesRegex(ValueError, expected_text):
            hf.validate_w2dsr_json(config)

    def test_minimal_configuration_is_valid(self):
        self.assertTrue(hf.validate_w2dsr_json(minimal_config()))

    def test_nonfinite_cell_parameter_is_rejected(self):
        config = minimal_config()
        config["nervous_system"]["cells"]["A_0"]["bias"]["value"] = math.inf
        self.assert_validation_error(config, "must be finite")

    def test_unknown_nervous_system_endpoint_is_rejected(self):
        config = minimal_config()
        config["nervous_system"]["chemical_conns"]["value"].append(
            {
                "from": "A_0",
                "to": "missing_cell",
                "weight": {"value": 1.0},
            }
        )
        self.assert_validation_error(config, "unknown cell 'missing_cell'")

    def test_invalid_sensor_references_are_rejected(self):
        config = minimal_config()
        config["sensors"] = {
            "sensor": {
                "environment": {"value": "missing_environment"},
                "sensor_m": {"value": 1.0},
                "sensor_n": {"value": 1.0},
                "outputs": {"value": [{"name": "output_1"}]},
                "weights": {
                    "value": [
                        {
                            "from_output": 2,
                            "to_cell": "A_0",
                            "weight": {"value": 1.0},
                        }
                    ]
                },
            }
        }
        self.assert_validation_error(config, "unknown environment")
        self.assert_validation_error(config, "existing sensor output")

    def test_unknown_scheduled_input_pattern_is_rejected(self):
        config = minimal_config()
        config["driving_inputs"] = {
            "inputs": {
                "value": [
                    {"input_num": "drive", "strength": {"value": 0.0}}
                ]
            },
            "weights": {"value": []},
        }
        config["input_switcher"] = {
            "inputs": {
                "value": [
                    {
                        "input_index": "pattern_1",
                        "value": [{"input_num": "drive", "value": 1.0}],
                    }
                ]
            },
            "size": {"value": 1},
            "time_periods": {"value": [1.0]},
            "input_indices": {"value": ["missing_pattern"]},
        }
        self.assert_validation_error(config, "unknown input pattern")

    def test_out_of_range_muscle_index_is_rejected(self):
        config = minimal_config()
        config["worm"] = {"N_muscles": {"value": 1}}
        config["dorsal_nmj"] = {
            "weights": {
                "value": [
                    {
                        "from_cell": "A_0",
                        "to_musc": 2,
                        "weight": {"value": 1.0},
                    }
                ]
            }
        }
        self.assert_validation_error(config, "exceeds the configured muscle count")

    def test_unknown_stretch_receptor_target_is_rejected(self):
        config = minimal_config()
        config["stretch_receptor"] = {
            "type": {"value": "SR18"},
            "set_direct": {"value": False},
            "ns_d_weights": {
                "value": [
                    {
                        "from_sr": 1,
                        "to_ns": "missing_cell",
                        "weight": {"value": 1.0},
                    }
                ]
            },
        }
        self.assert_validation_error(config, "unknown cell 'missing_cell'")

    def test_invalid_mfunc_is_rejected(self):
        config = minimal_config()
        parameter = config["nervous_system"]["cells"]["A_0"]["bias"]
        parameter["mfunc"] = {"f_ind": "mult_func", "fact": 0.0}
        self.assert_validation_error(config, "fact.*must be non-zero")

    def test_evotag_and_range_must_correspond(self):
        config = minimal_config()
        config["nervous_system"]["cells"]["A_0"]["bias"]["evotag"] = "bias"
        self.assert_validation_error(config, "missing from 'evolvable_ranges'")

        config = copy.deepcopy(config)
        config["evolvable_ranges"]["bias"] = {
            "active": True,
            "lower_limit": -1.0,
            "upper_limit": 1.0,
            "name": "bias",
        }
        self.assertTrue(hf.validate_w2dsr_json(config))


if __name__ == "__main__":
    unittest.main()
