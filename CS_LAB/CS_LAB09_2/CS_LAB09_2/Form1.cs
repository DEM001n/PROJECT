using System;
using System.Drawing;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CS_LAB09_2
{
    public partial class Form1 : Form
    {
        private CustomTimer customTimer;
        private readonly double[] angles; 
        private readonly double[] radii;  
        private readonly double[] speeds; 
        private readonly PictureBox[] planets; 

        public Form1()
        {
            InitializeComponent();

            pictureBox1.SizeMode = PictureBoxSizeMode.StretchImage;
            pictureBox2.SizeMode = PictureBoxSizeMode.StretchImage;
            pictureBox3.SizeMode = PictureBoxSizeMode.StretchImage;
            pictureBox4.SizeMode = PictureBoxSizeMode.StretchImage;
            pictureBox5.SizeMode = PictureBoxSizeMode.StretchImage;
            pictureBox6.SizeMode = PictureBoxSizeMode.StretchImage;

            planets = new PictureBox[] { pictureBox2, pictureBox3, pictureBox4, pictureBox5, pictureBox6 };

            angles = new double[planets.Length];
            radii = new double[] { 100, 150, 200, 250, 300 };
            speeds = new double[] { 5, 3, 2, 1.5, 1 };

            InitializeCustomTimer();
        }

        private void InitializeCustomTimer()
        {
            customTimer = new CustomTimer(100); // Interwał 100ms
            customTimer.TimeElapsed += UpdatePlanetPositions;
            customTimer.OnError += HandleError;
            customTimer.Start(); 
        }

        private void UpdatePlanetPositions()
        {
            if (InvokeRequired)
            {
                Invoke(new Action(UpdatePlanetPositions));
            }
            else
            {
                Point center = new Point(
                    pictureBox1.Location.X + pictureBox1.Width / 2,
                    pictureBox1.Location.Y + pictureBox1.Height / 2
                );

                for (int i = 0; i < planets.Length; i++)
                {
                    UpdatePlanetPosition(planets[i], radii[i], speeds[i], ref angles[i], center);
                }
            }
        }

        private void UpdatePlanetPosition(PictureBox planet, double radius, double speed, ref double angle, Point center)
        {
            angle += speed;
            if (angle >= 360) angle -= 360;

            double angleRad = angle * Math.PI / 180;

            int newX = center.X + (int)(radius * Math.Cos(angleRad)) - planet.Width / 2;
            int newY = center.Y + (int)(radius * Math.Sin(angleRad)) - planet.Height / 2;

            planet.Location = new Point(newX, newY);
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

        private void pictureBox1_Click(object sender, EventArgs e) {
            customTimer.SimulateError();
        }
        private void pictureBox2_Click(object sender, EventArgs e) { }

        private void pictureBox3_Click(object sender, EventArgs e) { }

        private void pictureBox4_Click(object sender, EventArgs e) { }

        private void pictureBox5_Click(object sender, EventArgs e) { }

        private void pictureBox6_Click(object sender, EventArgs e) { }
    }

    public class CustomTimer
    {
        private readonly int intervalMilliseconds;
        private CancellationTokenSource cancellationTokenSource;
        public event Action TimeElapsed;
        public event Action<Exception> OnError;

        public CustomTimer(int intervalMilliseconds)
        {
            if (intervalMilliseconds <= 0)
                throw new ArgumentException("Interval must be greater than 0", nameof(intervalMilliseconds));
            this.intervalMilliseconds = intervalMilliseconds;
        }

        public void Start()
        {
            Stop();
            cancellationTokenSource = new CancellationTokenSource();
            CancellationToken token = cancellationTokenSource.Token;

            Task.Run(async () =>
            {
                while (!token.IsCancellationRequested)
                {
                    try
                    {
                        await Task.Delay(intervalMilliseconds, token);
                        TimeElapsed?.Invoke();
                    }
                    catch (Exception ex) when (!(ex is TaskCanceledException))
                    {
                        OnError?.Invoke(ex);
                    }
                }
            }, token);
        }

        public void Stop()
        {
            cancellationTokenSource?.Cancel();
            cancellationTokenSource = null;
        }

        // Test method to simulate an error and trigger the OnError event.
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
