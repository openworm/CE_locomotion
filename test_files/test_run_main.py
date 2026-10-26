import argparse
import json
import tempfile
import unittest
from pathlib import Path

import run_main


class EvolutionGenerationFieldTests(unittest.TestCase):
    def test_cont_evol_defaults_to_false(self):
        self.assertFalse(run_main.DEFAULTS["contEvol"])

    def test_canonical_max_gens_is_preserved(self):
        evolution = {
            "maxGens": 20,
            "TotalGenerations": 160,
        }

        result = run_main.normalize_evolution_generation_fields(evolution)

        self.assertEqual(result["maxGens"], 20)
        self.assertEqual(result["TotalGenerations"], 160)

    def test_legacy_max_generations_is_accepted_but_not_rewritten(self):
        evolution = {"MaxGenerations": 20}

        result = run_main.normalize_evolution_generation_fields(evolution)

        self.assertEqual(result, {"maxGens": 20})

    def test_snake_case_alias_is_accepted_but_not_rewritten(self):
        evolution = {"max_generations": 30}

        result = run_main.normalize_evolution_generation_fields(evolution)

        self.assertEqual(result, {"maxGens": 30})


class EvolutionRunControlTests(unittest.TestCase):
    def _write_config(self, folder, **controls):
        evolution = {
            key: {"value": value} for key, value in controls.items()
        }
        path = Path(folder) / "worm_data_worm.json"
        path.write_text(json.dumps({"evolution": evolution}), encoding="utf-8")
        return path

    def test_do_evol_is_false_when_arguments_are_omitted(self):
        with tempfile.TemporaryDirectory() as folder:
            self._write_config(
                folder, doEvol=True, doCPT=False, contEvol=True
            )
            args = run_main.build_namespace(
                run_main.DEFAULTS,
                argparse.Namespace(inputFolderName=folder, outputFolderName="unused"),
            )

            controls = run_main._resolve_evolution_run_controls(args)

            self.assertEqual(
                controls,
                {"doEvol": False, "doCPT": False, "contEvol": True},
            )

    def test_do_evol_can_be_enabled_explicitly(self):
        with tempfile.TemporaryDirectory() as folder:
            self._write_config(folder, doEvol=False)
            args = run_main.build_namespace(
                run_main.DEFAULTS,
                inputFolderName=folder,
                outputFolderName="unused",
                doEvol=True,
            )

            controls = run_main._resolve_evolution_run_controls(args)

            self.assertTrue(controls["doEvol"])

    def test_explicit_arguments_override_json_controls(self):
        with tempfile.TemporaryDirectory() as folder:
            self._write_config(
                folder, doEvol=True, doCPT=True, contEvol=True
            )
            args = run_main.build_namespace(
                run_main.DEFAULTS,
                inputFolderName=folder,
                outputFolderName="unused",
                doEvol=False,
                doCPT=False,
                contEvol=False,
            )

            controls = run_main._resolve_evolution_run_controls(args)

            self.assertEqual(
                controls,
                {"doEvol": False, "doCPT": False, "contEvol": False},
            )

    def test_effective_controls_are_stored_in_all_configuration_files(self):
        with tempfile.TemporaryDirectory() as folder:
            first = self._write_config(folder, doEvol=False)
            second = Path(folder) / "worm_data_evo.json"
            second.write_text('{"evolution": {}}', encoding="utf-8")
            controls = {"doEvol": True, "doCPT": True, "contEvol": False}

            run_main._store_evolution_run_controls(folder, controls)

            for path in (first, second):
                evolution = json.loads(path.read_text(encoding="utf-8"))["evolution"]
                self.assertEqual(
                    {key: evolution[key]["value"] for key in controls}, controls
                )

    def test_storing_controls_preserves_legacy_evolution_parameters(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "worm_data_worm.json"
            path.write_text(
                json.dumps(
                    {
                        "Evolutionary Optimization Parameters": {
                            "PopulationSize": {"value": 16}
                        }
                    }
                ),
                encoding="utf-8",
            )

            run_main._store_evolution_run_controls(
                folder, {"doEvol": True, "doCPT": False, "contEvol": False}
            )

            config = json.loads(path.read_text(encoding="utf-8"))
            self.assertNotIn("Evolutionary Optimization Parameters", config)
            self.assertEqual(config["evolution"]["PopulationSize"]["value"], 16)


if __name__ == "__main__":
    unittest.main()
