package sciens.termacs;

/** Decoration style for the actuator widgets — Button, ComboBox, LineEdit (§5.12).
 *  Orthogonal to the color {@link Theme}: this is glyphs/geometry, the theme is colors.
 *  {@code INHERIT} (per-widget default) resolves to the app-wide global default.
 *  Ordinal matches TmControlStyle in the C ABI. */
public enum ControlStyle { INHERIT, PLAIN, BRACKETS, FRAMED }
