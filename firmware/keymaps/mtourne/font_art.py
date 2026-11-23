"""Draw the OLED modifier icons into glcdfont.c.

The font has 6x8 pixel cells. The cocot logo's cells (0x80-0xD4) are unused
since the logo was removed, so the icons go there; the font's size is fixed,
so this costs no flash. Edit the grids below and run:

    python3 font_art.py          # rewrite glcdfont.c and print a preview

Each modifier icon is 12x16 pixels: 2 cells wide, 2 rows tall. Icon i (0
Ctrl, 1 Option, 2 Command, 3 Shift) uses cells 0x80 + 4i: top-left,
top-right, bottom-left, bottom-right.

The OS icons are 12x16 too, at 0x90 (Apple, outline) and 0x94 (Windows),
same cell order.
"""

FONT_START = 0x20  # first character kept in glcdfont.c (OLED_FONT_START)
MOD_BASE = 0x80

# 12x12 icons with 2-pixel strokes, centred in 12x16 when drawn.
# CAGS order (pinky to index): Ctrl, Option, Command, Shift.
MODS = [
    [  # ⎈ Control (helm)
        ".....##.....",
        ".#...##...#.",
        "..########..",
        "..##....##..",
        ".##......##.",
        "###......###",
        "###......###",
        ".##......##.",
        "..##....##..",
        "..########..",
        ".#...##...#.",
        ".....##.....",
    ],
    [  # ⌥ Option
        "............",
        "............",
        "#####..#####",
        "#####..#####",
        "....##......",
        ".....##.....",
        "......##....",
        ".......##...",
        "........####",
        "........####",
        "............",
        "............",
    ],
    [  # ⌘ Command
        ".###....###.",
        "##.##..##.##",
        "##.##..##.##",
        ".##########.",
        "...##..##...",
        "...##..##...",
        "...##..##...",
        "...##..##...",
        ".##########.",
        "##.##..##.##",
        "##.##..##.##",
        ".###....###.",
    ],
    [  # ⇧ Shift
        ".....##.....",
        "....####....",
        "...##..##...",
        "..##....##..",
        ".##......##.",
        "####....####",
        "..##....##..",
        "..##....##..",
        "..##....##..",
        "..##....##..",
        "..########..",
        "..########..",
    ],
]


# 12x16 OS icons, at 0x90 (Apple) and 0x94 (Windows).
OS_BASE = 0x90
OS_ICONS = [
    [  # Apple, outline, bite on the right
        "............",
        "......##....",
        ".....##.....",
        ".....#......",
        "..###.####..",
        ".#........#.",
        "#........#..",
        "#.......#...",
        "#.......#...",
        "#........#..",
        ".#........#.",
        ".#.......#..",
        "..#.....#...",
        "...##.##....",
        "............",
        "............",
    ],
    [  # Windows, four panes
        "............",
        "............",
        "#####.#####.",
        "#####.#####.",
        "#####.#####.",
        "#####.#####.",
        "#####.#####.",
        "............",
        "#####.#####.",
        "#####.#####.",
        "#####.#####.",
        "#####.#####.",
        "#####.#####.",
        "............",
        "............",
        "............",
    ],
]


def padded(icon):
    """12x12 icon centred in 12x16."""
    blank = "." * 12
    return [blank, blank] + icon + [blank, blank]


def cell(grid, top, left):
    """Font bytes for the 6x8 cell at (top, left): one byte per column, bit 0 on top."""
    return [sum(1 << b for b in range(8) if grid[top + b][left + c] == "#") for c in range(6)]


def main():
    glyphs = {}
    for i, icon in enumerate(MODS):
        grid = padded(icon)
        for part, (top, left) in enumerate(((0, 0), (0, 6), (8, 0), (8, 6))):
            glyphs[MOD_BASE + 4 * i + part] = cell(grid, top, left)
    for i, icon in enumerate(OS_ICONS):
        for part, (top, left) in enumerate(((0, 0), (0, 6), (8, 0), (8, 6))):
            glyphs[OS_BASE + 4 * i + part] = cell(icon, top, left)
        print("\n".join(r.replace(".", " ") for r in icon))

    lines = open("glcdfont.c").read().split("\n")
    first = next(i for i, line in enumerate(lines) if line.startswith("0x"))
    for code, cols in glyphs.items():
        lines[first + code - FONT_START] = ", ".join("0x%02X" % v for v in cols) + ",  // 0x%02X art" % code
    open("glcdfont.c", "w").write("\n".join(lines))


if __name__ == "__main__":
    main()
