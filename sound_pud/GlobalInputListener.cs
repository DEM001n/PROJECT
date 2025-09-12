using System;
using System.Collections.Generic;
using System.Windows.Forms;
using Gma.System.MouseKeyHook;

namespace sound_pud
{
    using DTO;

    public class GlobalInputListener
    {
        private IKeyboardMouseEvents _globalHook;

        // Хранение состояния клавиш (для фильтрации повторов)
        private readonly HashSet<Keys> _pressedKeys = new HashSet<Keys>();
        private readonly HashSet<MouseButtons> _pressedButtons = new HashSet<MouseButtons>();

        // Событие для передачи информации наружу
        public event EventHandler<InputEventArgs> InputReceived;

        public void Start()
        {
            if (_globalHook != null) return;

            _globalHook = Hook.GlobalEvents();

            // Клавиатура
            _globalHook.KeyDown += OnKeyDown;
            _globalHook.KeyUp += OnKeyUp;

            // Мышь
            _globalHook.MouseDown += OnMouseDown;
            _globalHook.MouseUp += OnMouseUp;
        }

        public void Stop()
        {
            if (_globalHook == null) return;

            _globalHook.KeyDown -= OnKeyDown;
            _globalHook.KeyUp -= OnKeyUp;
            _globalHook.MouseDown -= OnMouseDown;
            _globalHook.MouseUp -= OnMouseUp;

            _globalHook.Dispose();
            _globalHook = null;

            _pressedKeys.Clear();
            _pressedButtons.Clear();
        }

        private void OnKeyDown(object sender, KeyEventArgs e)
        {
            if (_pressedKeys.Contains(e.KeyCode)) return; // Игнорируем повторные
            _pressedKeys.Add(e.KeyCode);



            InputReceived?.Invoke(this, new InputEventArgs("Keyboard", (int)e.KeyCode, true));
        }

        private void OnKeyUp(object sender, KeyEventArgs e)
        {
            if (!_pressedKeys.Contains(e.KeyCode)) return; // Игнорируем "лишние" отжатия
            _pressedKeys.Remove(e.KeyCode);

            InputReceived?.Invoke(this, new InputEventArgs("Keyboard", (int)e.KeyCode, false));
        }

        private void OnMouseDown(object sender, MouseEventArgs e)
        {
            if (_pressedButtons.Contains(e.Button)) return;
            _pressedButtons.Add(e.Button);

            InputReceived?.Invoke(this, new InputEventArgs("Mouse", (int)e.Button, true));
        }

        private void OnMouseUp(object sender, MouseEventArgs e)
        {
            if (!_pressedButtons.Contains(e.Button)) return;
            _pressedButtons.Remove(e.Button);

            InputReceived?.Invoke(this, new InputEventArgs("Mouse", (int)e.Button, false));
        }
    }

}