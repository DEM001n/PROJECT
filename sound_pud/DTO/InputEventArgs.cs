using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace sound_pud.DTO
{
    public class InputEventArgs : EventArgs
    {
        public string Device { get; }          // "Keyboard" или "Mouse"
        public int Key { get; }               // Клавиша
        public bool IsPressed { get; }         // true = нажата, false = отжата

        public InputEventArgs(string device, int key, bool isPressed)
        {
            Device = device;
            Key = key;
            IsPressed = isPressed;
        }

        public override string ToString()
        {
            return $"{Device}: {Key} - {(IsPressed ? "Down" : "Up")}";
        }
    }

}
