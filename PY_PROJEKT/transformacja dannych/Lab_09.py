import soundfile as sf
import numpy as np
import matplotlib.pyplot as plt
import librosa
import scipy.fftpack
from scipy.signal import find_peaks

def zad1():
    s, fs = sf.read('audio2.wav', dtype='float32')

    czas_trwania = len(s) / fs
    kanaly = 1 if s.ndim == 1 else s.shape[1]
    rozdzielczosc_bitowa = s.dtype.itemsize * 8
    print(f"Częstotliwość próbkowania: {fs} Hz")
    print(f"Czas trwania: {czas_trwania:.2f} sekundy")
    print(f"Liczba kanałów: {kanaly}")
    print(f"Rozdzielczość bitowa: {rozdzielczosc_bitowa} bitów")

    if kanaly > 1:
        s = s[:, 0]

    max_val = np.max(np.abs(s))
    if max_val > 1:
        s = s / max_val

    t = np.linspace(0, len(s) / fs, num=len(s)) * 1000

    plt.figure(figsize=(10, 4))
    plt.plot(t, s)
    plt.title("Sygnał audio (jeden kanał)")
    plt.xlabel("Czas [ms]")
    plt.ylabel("Amplituda")
    plt.grid()
    plt.tight_layout()
    plt.show()

    if np.any(np.abs(s) >= 1.0):
        print("Wykryto przesterowanie (clipping)!")
    else:
        print("Brak przesterowania.")

    min_amp = np.min(s)
    max_amp = np.max(s)
    print(f"Zakres amplitudy: {min_amp:.2f} do {max_amp:.2f}")

    # Szacowanie amplitudy szumu - chatgpt
    sample_range = int(0.5 * fs)
    szum_start = s[:sample_range]
    szum_end = s[-sample_range:]
    rms_start = np.sqrt(np.mean(szum_start ** 2))
    rms_end = np.sqrt(np.mean(szum_end ** 2))
    print(f"Amplituda szumu na początku: {rms_start:.4f}")
    print(f"Amplituda szumu na końcu: {rms_end:.4f}")

    return s,fs

def zad2(s, fs, ramka_ms=10, nakladanie=0.0):
    ramka_len = int(fs * ramka_ms / 1000)
    przesuniecie = int(ramka_len * (1 - nakladanie))

    liczba_ramek = int((len(s) - ramka_len) / przesuniecie) + 1

    E = []
    Z = []

    for i in range(liczba_ramek):
        start = i * przesuniecie
        end = start + ramka_len
        ramka = s[start:end]

        if len(ramka) < ramka_len:
            break

        energia = np.sum(ramka ** 2)
        E.append(energia)

        znaki = ramka[:-1] * ramka[1:]
        z = np.sum(znaki < 0)
        Z.append(z)

    E = np.array(E)
    Z = np.array(Z)

    E_norm = E / np.max(E)
    Z_norm = Z / np.max(Z)

    t_ramek = np.arange(liczba_ramek) * przesuniecie / fs * 1000

    plt.figure(figsize=(12, 6))
    t = np.linspace(0, len(s) / fs, len(s)) * 1000
    plt.plot(t, s, label='Sygnał', color='gray', alpha=0.6)
    plt.plot(t_ramek, E_norm, 'r-', label='Energia (E)')
    plt.plot(t_ramek, Z_norm, 'b-', label='Zero-crossings (Z)')
    plt.title(f"Funkcje E i Z dla długości ramki {ramka_ms} ms")
    plt.xlabel("Czas [ms]")
    plt.legend()
    plt.grid()
    plt.tight_layout()
    plt.show()

def zad3(s, fs):
    start_time = 2.2
    start_sample = int(start_time * fs)
    fragment = s[start_sample:start_sample + 2048]

    yf = scipy.fftpack.fft(fragment)
    N = len(yf)
    mag = np.abs(yf[:N // 2])
    mag_db = np.log10(mag + 1e-12)

    freqs = np.linspace(0, fs / 2, N // 2)

    plt.figure(figsize=(10, 4))
    plt.plot(freqs, mag_db)
    plt.title("Logarytmiczne widmo amplitudowe")
    plt.xlabel("Częstotliwość [Hz]")
    plt.ylabel("Amplituda [dB]")
    plt.xlim(0, 10000)
    plt.grid(True)
    plt.tight_layout()
    plt.show()

    peaks, _ = find_peaks(mag_db, height=np.max(mag_db) - 20)
    if len(peaks) > 0:
        f0_hz = freqs[peaks[0]]
        print(f"Częstotliwość podstawowa (F0): {f0_hz:.1f} Hz")
    else:
        print("Nie wykryto wyraźnej częstotliwości F0.")

    return fragment

def find_peaks_custom(spectrum):
    indeksy_pikow = []

    for i in range(1, len(spectrum) - 1):
        a, b, c = spectrum[i - 1], spectrum[i], spectrum[i + 1]
        if a <= b and b > c:
            indeksy_pikow.append(i)

    return indeksy_pikow
def rozpoznaj_samogloske(F1, F2):
    samogloski = {
        "a": (800, 1300),
        "e": (600, 1900),
        "i": (250, 2500),
        "o": (550, 900),
        "u": (300, 700),
        "y": (350, 2000)
    }

    min_dist = float('inf')
    najlepsza_samogloska = None

    for sgl, (f1_ref, f2_ref) in samogloski.items():
        dist = np.sqrt((F1 - f1_ref) ** 2 + (F2 - f2_ref) ** 2)
        if dist < min_dist:
            min_dist = dist
            najlepsza_samogloska = sgl

    print(f"\nRozpoznana samogłoska: '{najlepsza_samogloska.upper()}' (odległość: {min_dist:.1f})")
def zad4(fs, okno):
    a = librosa.lpc(okno, order=20)

    '''
    Liniowe Kodowanie Predykcyjne (LPC) modeluje widmo mowy poprzez predykcję próbki sygnału na podstawie poprzednich.
    Umożliwia wygładzenie widma i wyznaczanie formantów charakterystycznych dla samogłosek.
    '''

    a_padded = np.concatenate([a, np.zeros(2048 - len(a))])

    widmo_LPC = -np.log10(np.abs(scipy.fftpack.fft(a_padded)) + 1e-12)
    yf = scipy.fftpack.fft(okno)
    widmo_sygnalu = 1.5 + np.log10(np.abs(yf) + 1e-12)

    N = len(okno)
    freqs = np.linspace(0, fs / 2, N // 2)

    plt.figure(figsize=(10, 5))
    plt.plot(freqs, widmo_sygnalu[:N // 2], label='Widmo sygnału (FFT)', color='blue')
    plt.plot(freqs, widmo_LPC[:N // 2], label='Widmo LPC (wygładzone)', color='red')
    plt.title("Widmo amplitudowe i widmo LPC")
    plt.xlabel("Częstotliwość [Hz]")
    plt.ylabel("Amplituda [dB]")
    plt.legend()
    plt.xlim(0, 10000)
    plt.grid(True)
    plt.tight_layout()
    plt.show()

    #peaks = find_peaks_custom(widmo_LPC[:N // 2])
    peaks, _ = find_peaks(widmo_LPC[:N // 2], distance=20)
    formanty_freq = freqs[peaks]
    formanty_freq = formanty_freq[formanty_freq < 6000]

    if len(formanty_freq) >= 2:
        F1, F2 = formanty_freq[:2]
        print(f"Formanty: F1 = {F1:.0f} Hz, F2 = {F2:.0f} Hz")

        rozpoznaj_samogloske(F1, F2)
    else:
        print("Nie udało się wykryć dwóch formantów.")

if __name__ == '__main__':
    zad1_data, fs = zad1()
    '''
    for ramka_ms in [5 ,10 ,20, 50]:
        zad2(zad1_data, fs, ramka_ms=ramka_ms)
      '''
    zad2(zad1_data, fs, ramka_ms=10, nakladanie=0.5)

    okno = zad3(zad1_data, fs)

    zad4(fs, okno)
