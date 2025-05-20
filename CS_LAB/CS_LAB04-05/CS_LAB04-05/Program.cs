public interface ISamochód
{
    void Uruchom();
    void Wyłącz();
    void Tankuj();
    void Jedź();
}

public interface ISamochódBenzyna : ISamochód
{
    bool SilnikSpalinowy { get; set; }
    bool Bak { get; set; }
}

public interface ISamochódGaz : ISamochód
{
    bool SilnikSpalinowy { get; set; }
    bool Butla { get; set; }
}

public interface ISamochódPrąd : ISamochód
{
    bool SilnikElektryczny { get; set; }
    bool Akumulator { get; set; }
}


public class SamochódBenzyna : ISamochódBenzyna
{
    public bool SilnikSpalinowy { get; set; } = false;
    public bool Bak { get; set; } = true;

    public void Uruchom()
    {
        if (Bak)
        {
            SilnikSpalinowy = true;
            Console.WriteLine("Uruchomiono silnik spalinowy (benzyna).");
        }
    }

    public void Wyłącz()
    {
        SilnikSpalinowy = false;
        Console.WriteLine("Wyłączono silnik spalinowy (benzyna).");
    }

    public void Tankuj()
    {
        Bak = true;
        Console.WriteLine("Zatankowano benzynę.");
    }

    public void Jedź()
    {
        if (SilnikSpalinowy && Bak)
        {
            Console.WriteLine("Jedziesz na benzynie.");
        }
        else
        {
            Console.WriteLine("Brak paliwa lub silnik nie jest uruchomiony.");
        }
    }
}

public class SamochódGaz : ISamochódGaz
{
    public bool SilnikSpalinowy { get; set; } = false;
    public bool Butla { get; set; } = true;

    public void Uruchom()
    {
        if (Butla)
        {
            SilnikSpalinowy = true;
            Console.WriteLine("Uruchomiono silnik spalinowy (gaz).");
        }
    }

    public void Wyłącz()
    {
        SilnikSpalinowy = false;
        Console.WriteLine("Wyłączono silnik spalinowy (gaz).");
    }

    public void Tankuj()
    {
        Butla = true;
        Console.WriteLine("Napełniono butlę gazem.");
    }

    public void Jedź()
    {
        if (SilnikSpalinowy && Butla)
        {
            Console.WriteLine("Jedziesz na gazie.");
        }
        else
        {
            Console.WriteLine("Brak gazu lub silnik nie jest uruchomiony.");
        }
    }
}

public class SamochódPrąd : ISamochódPrąd
{
    public bool SilnikElektryczny { get; set; } = false;
    public bool Akumulator { get; set; } = true;

    public void Uruchom()
    {
        if (Akumulator)
        {
            SilnikElektryczny = true;
            Console.WriteLine("Uruchomiono silnik elektryczny.");
        }
    }

    public void Wyłącz()
    {
        SilnikElektryczny = false;
        Console.WriteLine("Wyłączono silnik elektryczny.");
    }

    public void Tankuj()
    {
        Akumulator = true;
        Console.WriteLine("Naładowano akumulator.");
    }

    public void Jedź()
    {
        if (SilnikElektryczny && Akumulator)
        {
            Console.WriteLine("Jedziesz na prąd.");
        }
        else
        {
            Console.WriteLine("Akumulator rozładowany lub silnik nie jest uruchomiony.");
        }
    }
}

public class SamochódBenzynaGaz : ISamochódBenzyna, ISamochódGaz
{
    public bool SilnikSpalinowy { get; set; } = false;
    public bool Bak { get; set; } = true;
    public bool Butla { get; set; } = true;

    public void Uruchom()
    {
        if (Bak || Butla)
        {
            SilnikSpalinowy = true;
            Console.WriteLine("Uruchomiono silnik spalinowy (benzyna/gaz).");
        }
    }

    public void Wyłącz()
    {
        SilnikSpalinowy = false;
        Console.WriteLine("Wyłączono silnik spalinowy (benzyna/gaz).");
    }

    public void Tankuj()
    {
        Bak = true;
        Butla = true;
        Console.WriteLine("Zatankowano benzynę i gaz.");
    }

    public void Jedź()
    {
        if (SilnikSpalinowy && (Bak || Butla))
        {
            Console.WriteLine("Jedziesz na benzynie lub gazie.");
        }
        else
        {
            Console.WriteLine("Brak paliwa lub silnik nie jest uruchomiony.");
        }
    }
}

public class SamochódBenzynaPrąd : ISamochódBenzyna, ISamochódPrąd
{
    public bool SilnikSpalinowy { get; set; } = false;
    public bool Bak { get; set; } = true;
    public bool SilnikElektryczny { get; set; } = false;
    public bool Akumulator { get; set; } = true;

    public void Uruchom()
    {
        if (Bak)
        {
            SilnikSpalinowy = true;
            Console.WriteLine("Uruchomiono silnik spalinowy (benzyna).");
        }
        if (Akumulator)
        {
            SilnikElektryczny = true;
            Console.WriteLine("Uruchomiono silnik elektryczny.");
        }
    }

    public void Wyłącz()
    {
        SilnikSpalinowy = false;
        SilnikElektryczny = false;
        Console.WriteLine("Wyłączono oba silniki.");
    }

    public void Tankuj()
    {
        Bak = true;
        Akumulator = true;
        Console.WriteLine("Zatankowano benzynę i naładowano akumulator.");
    }

    public void Jedź()
    {
        if (SilnikSpalinowy && Bak)
        {
            Console.WriteLine("Jedziesz na benzynie.");
        }
        else if (SilnikElektryczny && Akumulator)
        {
            Console.WriteLine("Jedziesz na prąd.");
        }
        else
        {
            Console.WriteLine("Brak paliwa lub oba silniki są wyłączone.");
        }
    }
}


class Program
{
    static void Main(string[] args)
    { 
            var car = new SamochódBenzynaPrąd();

            car.Uruchom();  
            ((ISamochód)car).Uruchom();  
            ((ISamochódPrąd)car).Uruchom();  
            ((ISamochódBenzyna)car).Uruchom(); 
    }
}
