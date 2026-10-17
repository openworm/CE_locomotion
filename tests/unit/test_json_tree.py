import unittest

from worm2d import helper_funcs as hf


class JSONTreeTests(unittest.TestCase):
    def setUp(self):
        if hf.widgets is None:
            self.skipTest("ipywidgets is not installed")
        self.data = {
            "nervous_system": {
                "chemical_conns": [
                    {"from": "A", "to": "B", "weight": 1.0},
                    {"from": "B", "to": "C", "weight": -2.0},
                    {"from": "C", "to": "D", "weight": 3.0},
                ]
            }
        }

    def test_tree_previews_objects_and_updates_for_search(self):
        widget = hf.display_json_tree(
            self.data,
            expanded_depth=0,
            list_preview=2,
        )
        search, tree = widget.children
        self.assertIn("chemical_conns", tree.value)
        self.assertIn("json-preview", tree.value)
        self.assertIn("json-ellipsis", tree.value)
        self.assertIn("[0]", tree.value)
        self.assertIn("[1]", tree.value)
        self.assertIn("[2]", tree.value)

        search.value = "-2.0"
        self.assertIn("-2.0", tree.value)
        self.assertNotIn("[0]", tree.value)
        self.assertIn("[1]", tree.value)
        self.assertNotIn("[2]", tree.value)

    def test_tree_can_be_returned_without_search_controls(self):
        tree = hf.display_json_tree(self.data, searchable=False)
        self.assertIsInstance(tree, hf.widgets.HTML)

    def test_tree_rejects_invalid_display_options(self):
        with self.assertRaises(TypeError):
            hf.display_json_tree("not a container")
        with self.assertRaises(ValueError):
            hf.display_json_tree(self.data, expanded_depth=-1)
        with self.assertRaises(ValueError):
            hf.display_json_tree(self.data, list_preview=-1)


if __name__ == "__main__":
    unittest.main()
