using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CS_LIBRARY12
{

    public class CustomBackgroundWorker
    {
        private readonly ConcurrentQueue<Action> _eventQueue = new ConcurrentQueue<Action>();
        private CancellationTokenSource _cancellationTokenSource;

        public event Action<int> ProgressChanged;
        public event Action<Exception> OnError;
        public event Action OnCompleted;

        //OnCompleted 
        public void RunWorkerAsync(Action<object> userFunction, object parameter)
        {
            _cancellationTokenSource = new CancellationTokenSource();
            CancellationToken token = _cancellationTokenSource.Token;

            Task.Run(() =>
            {
                try
                {
                    userFunction(parameter); // Wywołanie funkcji użytkownika
                }
                catch (Exception ex)
                {
                    EnqueueError(ex);
                }
                finally
                {
                    OnCompleted?.Invoke();
                }
            }, token);
        }

        public void ReportProgress(int progress)
        {
            _eventQueue.Enqueue(() => ProgressChanged?.Invoke(progress));
        }

        public void EnqueueError(Exception ex)
        {
            _eventQueue.Enqueue(() => OnError?.Invoke(ex));
        }

        public void Cancel()
        {
            _cancellationTokenSource?.Cancel();
        }

        public void ProcessQueue()
        {
            while (_eventQueue.TryDequeue(out var action))
            {
                action();
            }
        }
    }
}
