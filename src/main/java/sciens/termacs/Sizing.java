package sciens.termacs;

/** Per-axis sizing constraint. {@code max == 0} means unbounded (the ABI maps it). */
public final class Sizing {
    public int min = 0, preferred = 0, max = 0, stretch = 0;
    public static Sizing of() { return new Sizing(); }
    public Sizing min(int v)       { min = v; return this; }
    public Sizing preferred(int v) { preferred = v; return this; }
    public Sizing max(int v)       { max = v; return this; }
    public Sizing stretch(int v)   { stretch = v; return this; }
}
