using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace sound_pud.DTO
{
    public class KeyPlayerInfo
    {
        public SoundPlayerManager Player { get; set; } // сам объект проигрывателя
        public CancellationTokenSource TokenSource { get; set; } // токен для остановки
    }
}
