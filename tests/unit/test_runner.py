import unittest

from worm2d import runner


class EvolutionGenerationFieldTests(unittest.TestCase):
    def test_cont_evol_defaults_to_false(self):
        self.assertFalse(runner.DEFAULTS["contEvol"])

    def test_canonical_max_gens_is_preserved(self):
        evolution = {
            "maxGens": 20,
            "TotalGenerations": 160,
        }

        result = runner.normalize_evolution_generation_fields(evolution)

        self.assertEqual(result["maxGens"], 20)
        self.assertEqual(result["TotalGenerations"], 160)

    def test_legacy_max_generations_is_accepted_but_not_rewritten(self):
        evolution = {"MaxGenerations": 20}

        result = runner.normalize_evolution_generation_fields(evolution)

        self.assertEqual(result, {"maxGens": 20})

    def test_snake_case_alias_is_accepted_but_not_rewritten(self):
        evolution = {"max_generations": 30}

        result = runner.normalize_evolution_generation_fields(evolution)

        self.assertEqual(result, {"maxGens": 30})


if __name__ == "__main__":
    unittest.main()
