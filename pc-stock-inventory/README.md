# PC Stock Inventory (C)

A small console inventory system for computer parts, written in standard C.
Products are grouped by category, and the program alerts you when an item is
**running low** or **out of stock**.

[ภาษาไทย](README.th.md)

## Features

- 8 categories: CPU, Motherboard, RAM, GPU, Storage, Power Supply, Monitor, Accessories
- Low-stock alert when `qty <= min` (each product has its own threshold)
- Out-of-stock alert when `qty == 0`
- Alerts shown at startup and right after a receive/issue changes an item's status
- Receive stock, issue stock (cannot issue more than is in stock)
- Add, delete products and change thresholds
- Data saved automatically to `stock.txt`
- No external dependencies (C99)

## Build and run

```bash
make
./pc_stock
```

Or without `make`:

```bash
gcc -std=c99 -Wall -Wextra -o pc_stock src/pc_stock.c
```

On first run the program creates 10 sample products in `stock.txt`.

## Menu

| Option | Action |
|-------:|--------|
| 1 | Show all products |
| 2 | Show by category |
| 3 | Add new product |
| 4 | Receive stock |
| 5 | Issue stock |
| 6 | View alerts (low / out of stock) |
| 7 | Change low-stock threshold |
| 8 | Delete product |
| 0 | Save and exit |

## Example output

```
ID   Product                      Category          Qty    Min  Status
--------------------------------------------------------------------------
1    Intel Core i5-14400F         CPU                14      5  [OK]
2    AMD Ryzen 5 5600             CPU                 0      5  [OUT OF STOCK]
5    Corsair Vengeance DDR4 8GB   RAM                 6      8  [LOW]
```

## Data file format

One product per line, fields separated by `|`:

```
id|category_index|quantity|min_threshold|name
1|0|14|5|Intel Core i5-14400F
```

Product names must not contain `|`. Category indexes refer to the `CATS`
array in `src/pc_stock.c`; add new categories at the end only, so existing
products keep their category.

## Project layout

```
.
├── src/pc_stock.c          # source code
├── docs/code-guide-th.pdf  # Thai documentation: analysis, flowcharts, code guide
├── Makefile
├── .github/workflows/      # CI: compiles on every push
└── LICENSE
```

## Limits

- Up to 200 products (`MAX_ITEMS`)
- Product names up to 59 characters

## License

MIT, see [LICENSE](LICENSE).
