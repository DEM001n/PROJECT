using System;
using System.Collections.Generic;

public interface ISamochod
{
    void Uruchom();
    void Wyłącz();
    void Tankuj();
    void Jedz();
}

public interface ISamochodBenzyna
{
    void TankujBenzyna();
}

public interface ISamochodGaz
{
    void TankujGaz();
}

public interface ISamochodPrad
{
    void Ladowanie();
}

public class SamochodBenzyna : ISamochod, ISamochodBenzyna
{
    private bool uruchomiony = false;
    private bool zatankowany = false;

    public void Uruchom()
    {
        if (uruchomiony)
        {
            Console.WriteLine("Samochód jest już uruchomiony.");
        }
        else
        {
            uruchomiony = true;
            Console.WriteLine("Uruchamiam samochód.");
        }
    }

    public void Wyłącz()
    {
        if (uruchomiony)
        {
            uruchomiony = false;
            Console.WriteLine("Wyłączam silnik.");
        }
        else
        {
            Console.WriteLine("Silnik jest wyłączony.");
        }
    }

    public void Tankuj()
    {
        TankujBenzyna();
    }

    public void TankujBenzyna()
    {
        if (zatankowany)
        {
            Console.WriteLine("Samochód już zatankowany benzyną.");
        }
        else
        {
            zatankowany = true;
            Console.WriteLine("Tankuję benzyną.");
        }
    }

    public void Jedz()
    {
        if (!zatankowany)
        {
            Console.WriteLine("Brak paliwa, zatankuj!");
            return;
        }

        if (!uruchomiony)
        {
            Console.WriteLine("Samochód nie jest uruchomiony.");
            return;
        }

        Console.WriteLine("Jadę na benzynę.");
        zatankowany = false;
    }
}

public class SamochodGaz : ISamochod, ISamochodGaz
{
    private bool uruchomiony = false;
    private bool zatankowany = false;

    public void Uruchom()
    {
        if (uruchomiony)
        {
            Console.WriteLine("Samochód jest już uruchomiony.");
        }
        else
        {
            uruchomiony = true;
            Console.WriteLine("Uruchamiam samochód.");
        }
    }

    public void Wyłącz()
    {
        if (uruchomiony)
        {
            uruchomiony = false;
            Console.WriteLine("Wyłączam silnik.");
        }
        else
        {
            Console.WriteLine("Silnik jest wyłączony.");
        }
    }

    public void Tankuj()
    {
        TankujGaz();
    }

    public void TankujGaz()
    {
        if (zatankowany)
        {
            Console.WriteLine("Samochód już zatankowany gazem.");
        }
        else
        {
            zatankowany = true;
            Console.WriteLine("Tankuję gaz.");
        }
    }

    public void Jedz()
    {
        if (!zatankowany)
        {
            Console.WriteLine("Brak paliwa, zatankuj!");
            return;
        }

        if (!uruchomiony)
        {
            Console.WriteLine("Samochód nie jest uruchomiony.");
            return;
        }

        Console.WriteLine("Jadę na gaz.");
        zatankowany = false;
    }
}

public class SamochodPrad : ISamochod, ISamochodPrad
{
    private bool uruchomiony = false;
    private bool zatankowany = false;

    public void Uruchom()
    {
        if (uruchomiony)
        {
            Console.WriteLine("Samochód jest już uruchomiony.");
        }
        else
        {
            uruchomiony = true;
            Console.WriteLine("Uruchamiam samochód.");
        }
    }

    public void Wyłącz()
    {
        if (uruchomiony)
        {
            uruchomiony = false;
            Console.WriteLine("Wyłączam silnik.");
        }
        else
        {
            Console.WriteLine("Silnik jest wyłączony.");
        }
    }

    public void Tankuj()
    {
        Ladowanie();
    }

    public void Ladowanie()
    {
        if (zatankowany)
        {
            Console.WriteLine("Akumulatory są już naładowane.");
        }
        else
        {
            zatankowany = true;
            Console.WriteLine("Ładuję akumulatory.");
        }
    }

    public void Jedz()
    {
        if (!zatankowany)
        {
            Console.WriteLine("Brak prądu, naładuj akumulatory!");
            return;
        }

        if (!uruchomiony)
        {
            Console.WriteLine("Samochód nie jest uruchomiony.");
            return;
        }

        Console.WriteLine("Jadę na prąd.");
        zatankowany = false;
    }
}

public class SamochodBenzynaGaz : ISamochod, ISamochodBenzyna, ISamochodGaz
{
    private bool uruchomiony = false;
    private bool zatankowanyBenzyna = false;
    private bool zatankowanyGaz = false;

    public void Uruchom()
    {
        if (uruchomiony)
        {
            Console.WriteLine("Samochód jest już uruchomiony.");
        }
        else
        {
            uruchomiony = true;
            Console.WriteLine("Uruchamiam samochód.");
        }
    }

    public void Wyłącz()
    {
        if (uruchomiony)
        {
            uruchomiony = false;
            Console.WriteLine("Wyłączam silnik.");
        }
        else
        {
            Console.WriteLine("Silnik jest wyłączony.");
        }
    }

    void ISamochodBenzyna.TankujBenzyna()
    {
        if (zatankowanyBenzyna)
        {
            Console.WriteLine("Samochód juz zatankован benz.");
        }
        else
        {
            zatankowanyBenzyna = true;
            Console.WriteLine("tankuje benz.");
        }
    }

    // Реализация метода для газа через интерфейс ISamochodGaz
    void ISamochodGaz.TankujGaz()
    {
        if (zatankowanyGaz)
        {
            Console.WriteLine("Samochód juz заправлен gazem.");
        }
        else
        {
            zatankowanyGaz = true;
            Console.WriteLine("tankuje gazem.");
        }
    }

    // Метод Tankuj вызывает методы интерфейсов для заправки бензином и газом
    public void Tankuj()
    {
        ((ISamochodBenzyna)this).TankujBenzyna();
        ((ISamochodGaz)this).TankujGaz();
    }

    // Логика в методе Jedz осталась прежней
    public void Jedz()
    {
        if (zatankowanyBenzyna)
        {
            if (uruchomiony)
            {
                Console.WriteLine("jade na benzynie");
                zatankowanyBenzyna = false;
                return;
            }
            Console.WriteLine("silnik benzyniwy nie wlaczony");
        }

        if (zatankowanyGaz)
        {
            if (uruchomiony)
            {
                Console.WriteLine("jadu na gazie.");
                zatankowanyGaz = false;
                return;
            }
            Console.WriteLine("gazowy silnik nie wlaczony.");
        }

        Console.WriteLine("nie zatankowany");
    }
}


public class SamochodBenzynaPrad : ISamochod, ISamochodBenzyna, ISamochodPrad
{
    private bool uruchomiony = false;
    private bool zatankowanyBenzyna = false;
    private bool naładowanyAkumulator = false;

    public void Uruchom()
    {
        if (uruchomiony)
        {
            Console.WriteLine("Samochód jest już uruchomiony.");
        }
        else
        {
            uruchomiony = true;
            Console.WriteLine("Uruchamiam samochód.");
        }
    }

    public void Wyłącz()
    {
        if (uruchomiony)
        {
            uruchomiony = false;
            Console.WriteLine("Wyłączam silnik.");
        }
        else
        {
            Console.WriteLine("Silnik jest wyłączony.");
        }
    }

    public void Tankuj()
    {
        TankujBenzyna();
        Ladowanie();
    }

    public void TankujBenzyna()
    {
        if (zatankowanyBenzyna)
        {
            Console.WriteLine("Samochód już zatankowany benzyną.");
        }
        else
        {
            zatankowanyBenzyna = true;
            Console.WriteLine("Tankuję benzyną.");
        }
    }

    public void Ladowanie()
    {
        if (naładowanyAkumulator)
        {
            Console.WriteLine("Akumulatory są już naładowane.");
        }
        else
        {
            naładowanyAkumulator = true;
            Console.WriteLine("Ładuję akumulatory.");
        }
    }

    public void Jedz()
    {
        if (!zatankowanyBenzyna && !naładowanyAkumulator)
        {
            Console.WriteLine("Brak paliwa i prądu, zatankuj!");
            return;
        }

        if (!uruchomiony)
        {
            Console.WriteLine("Samochód nie jest uruchomiony.");
            return;
        }

        if (zatankowanyBenzyna)
        {
                Console.WriteLine("Jadę na benzynę.");
                zatankowanyBenzyna = false;
                return;
            }

            if (naładowanyAkumulator)
            {
                Console.WriteLine("Jadę na prąd.");
                naładowanyAkumulator = false;
                return;
            }
        }
    }

public class SamochodGazPrad : ISamochod, ISamochodGaz, ISamochodPrad
    {
        private bool uruchomiony = false;
        private bool zatankowanyGaz = false;
        private bool naładowanyAkumulator = false;

        public void Uruchom()
        {
            if (uruchomiony)
            {
                Console.WriteLine("Samochód jest już uruchomiony.");
            }
            else
            {
                uruchomiony = true;
                Console.WriteLine("Uruchamiam samochód.");
            }
        }

        public void Wyłącz()
        {
            if (uruchomiony)
            {
                uruchomiony = false;
                Console.WriteLine("Wyłączam silnik.");
            }
            else
            {
                Console.WriteLine("Silnik jest wyłączony.");
            }
        }

        public void Tankuj()
        {
            TankujGaz();
            Ladowanie();
        }

        public void TankujGaz()
        {
            if (zatankowanyGaz)
            {
                Console.WriteLine("Samochód już zatankowany gazem.");
            }
            else
            {
                zatankowanyGaz = true;
                Console.WriteLine("Tankuję gaz.");
            }
        }

        public void Ladowanie()
        {
            if (naładowanyAkumulator)
            {
                Console.WriteLine("Akumulatory są już naładowane.");
            }
            else
            {
                naładowanyAkumulator = true;
                Console.WriteLine("Ładuję akumulatory.");
            }
        }

        public void Jedz()
        {
            if (zatankowanyGaz)
            {
                if (uruchomiony)
                {
                    Console.WriteLine("Jadę na gazie.");
                    zatankowanyGaz = false;
                    return;
                }
                Console.WriteLine("Gazowy silnik nie jest uruchomiony.");
            }

            if (naładowanyAkumulator)
            {
                if (uruchomiony)
                {
                    Console.WriteLine("Jadę na prądzie.");
                    naładowanyAkumulator = false;
                    return;
                }
                Console.WriteLine("Elektryczny silnik nie jest uruchomiony.");
            }

            Console.WriteLine("Brak paliwa, zatankuj!");
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            List<ISamochod> samochody = new List<ISamochod>
        {
            new SamochodBenzyna(),
            new SamochodGaz(),
            new SamochodPrad(),
            new SamochodBenzynaGaz(),
            new SamochodBenzynaPrad(),
            new SamochodGazPrad()
        };

            foreach (ISamochod samochod in samochody)
            {
                JazdaTestowa(samochod);
                Console.WriteLine();
            }

            ISamochodBenzyna hybrydaBenzynaGaz = new SamochodBenzynaGaz();
            hybrydaBenzynaGaz.TankujBenzyna();

            ISamochod hybrydaBenzynaPrad = new SamochodBenzynaPrad();
            hybrydaBenzynaPrad.Tankuj();

            ISamochod hybrydaGazPrad = new SamochodGazPrad();
            hybrydaGazPrad.Tankuj();
        }

        static void JazdaTestowa(ISamochod samochod)
        {
            samochod.Tankuj();
            samochod.Tankuj();
            samochod.Jedz();
            samochod.Uruchom();
            samochod.Uruchom();
            samochod.Jedz();
            samochod.Jedz();
            samochod.Wyłącz();
            samochod.Wyłącz();
        }
    }

