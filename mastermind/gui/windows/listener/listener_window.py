import dearpygui.dearpygui as dpg
from models.window import Window


class ListenerWindow(Window):
    def window_data(self):
        # Refactor this later, for test purposes only.
        listeners = ["asd", "fgh"]

        with dpg.group(horizontal=True):
            dpg.add_text("Search:")
            dpg.add_input_text(hint="Search for a listener...")
            dpg.add_button(label="Create new listener", width=-1)

        with dpg.child_window():
                for listener in listeners:
                    dpg.add_text(listener)
                    