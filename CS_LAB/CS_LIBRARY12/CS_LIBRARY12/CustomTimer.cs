using System;
using System.Collections.Concurrent;
using System.Threading;
using System.Threading.Tasks;


namespace CS_LIBRARY12
{
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

