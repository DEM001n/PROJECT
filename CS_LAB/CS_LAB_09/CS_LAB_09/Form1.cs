using System;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CS_LAB_09
{
    public partial class Form1 : Form
    {
        private CustomTimer customTimer;

        public Form1()
        {
            InitializeComponent();
            InitializeCustomTimer();
        }

        private void InitializeCustomTimer()
        {
            customTimer = new CustomTimer(1000); // Interwał 1000ms = 1 sekunda
            customTimer.TimeElapsed += UpdateLabels;
            customTimer.OnError += HandleError;
            customTimer.Start();
        }

        private void UpdateLabels()
        {
            if (InvokeRequired)
            {
                Invoke(new Action(UpdateLabels));
            }
            else
            {
                label1.Text = DateTime.Now.ToString("yyyy-MM-dd");
                label2.Text = DateTime.Now.ToString("HH:mm:ss");
            }
        }

        private void HandleError(Exception ex)
        {
            customTimer.Stop();
            MessageBox.Show($"An error occurred: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            customTimer.Stop();
        }

        private void label2_Click(object sender, EventArgs e)
        {
            customTimer.SimulateError();
        }
        private void label1_Click(object sender, EventArgs e) { }
    }

    public class CustomTimer
    {
        private int intervalMilliseconds;
        private CancellationTokenSource cancellationTokenSource;

        public event Action TimeElapsed;
        public event Action<Exception> OnError;

        public void setIntervalMilliseconds(int intervalMilliseconds)
        {
            if (intervalMilliseconds <= 0)
                throw new ArgumentException("Interval must be greater than 0", nameof(intervalMilliseconds));
            this.intervalMilliseconds = intervalMilliseconds;
        }

        public CustomTimer(int intervalMilliseconds)
        {
            setIntervalMilliseconds(intervalMilliseconds);
        }

        public void Start()
        {
            Stop();
            cancellationTokenSource = new CancellationTokenSource();
            CancellationToken token = cancellationTokenSource.Token;

            Task.Run(() =>
            {
                while (!token.IsCancellationRequested)
                {
                    try
                    {
                        Task.Delay(intervalMilliseconds, token).Wait(token);
                        TimeElapsed?.Invoke();
                    }
                    catch (Exception ex)
                    {
                        if (ex is TaskCanceledException)
                            break;

                        OnError?.Invoke(ex);
                    }
                }
            });
        }

        public void Stop()
        {
            cancellationTokenSource?.Cancel();
            cancellationTokenSource = null;
        }

        // Метод для симуляции ошибки и вызова OnError
        public void SimulateError()
        {
            try
            {
                throw new InvalidOperationException("Simulated error.");
            }
            catch (Exception ex)
            {
                OnError?.Invoke(ex);
            }
        }
    }


}
