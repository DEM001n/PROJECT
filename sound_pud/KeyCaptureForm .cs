using System;
using System.Windows.Forms;

namespace sound_pud
{
    public partial class KeyCaptureForm : Form
    {
        private GlobalInputListener _listener;

        public int CapturedKey { get; private set; } = -1;

        public KeyCaptureForm()
        {
            InitializeComponent();

            // Инициализация хука
            _listener = new GlobalInputListener();
            _listener.InputReceived += Listener_InputReceived;
            _listener.Start();
        }

        private void Listener_InputReceived(object sender, DTO.InputEventArgs e)
        {
            CapturedKey = e.Key;
            DialogResult = DialogResult.OK;

            _listener.Stop(); // останавливаем хук
            this.Close();
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            base.OnFormClosing(e);
            _listener.Stop();
        }
    }
}
