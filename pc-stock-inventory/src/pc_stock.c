/*
 * Computer Parts Inventory System (C)
 * - Products are grouped by category
 * - Alerts when stock is low (qty <= min) and when sold out (qty == 0)
 * - Data is saved automatically to stock.txt
 *
 * Build:  gcc pc_stock.c -o pc_stock
 * Run:    ./pc_stock
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 200
#define NAME_LEN  60
#define NUM_CATS  8
#define DATA_FILE "stock.txt"

static const char *CATS[NUM_CATS] = {
    "CPU", "Motherboard", "RAM", "GPU",
    "Storage", "Power Supply", "Monitor", "Accessories"
};

typedef struct {
    int  id;
    int  cat;              /* index into CATS */
    int  qty;              /* quantity in stock */
    int  min;              /* low-stock alert threshold */
    char name[NAME_LEN];
} Product;

static Product items[MAX_ITEMS];
static int count = 0;
static int nextId = 1;

/* ---------- Input helpers ---------- */
static void readLine(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    buf[strcspn(buf, "\r\n")] = '\0';
}

static int readInt(const char *prompt, int lo, int hi) {
    char buf[64];
    char *end;
    for (;;) {
        readLine(prompt, buf, sizeof buf);
        long v = strtol(buf, &end, 10);
        if (end != buf && *end == '\0' && v >= lo && v <= hi) return (int)v;
        printf("  Please enter a number between %d and %d.\n", lo, hi);
    }
}

static int pickCategory(void) {
    for (int i = 0; i < NUM_CATS; i++) printf("  %d) %s\n", i + 1, CATS[i]);
    return readInt("Choose category: ", 1, NUM_CATS) - 1;
}

/* ---------- Stock status ---------- */
static const char *statusText(const Product *p) {
    if (p->qty <= 0)      return "[OUT OF STOCK]";
    if (p->qty <= p->min) return "[LOW]";
    return "[OK]";
}

static int findById(int id) {
    for (int i = 0; i < count; i++) if (items[i].id == id) return i;
    return -1;
}

/* ---------- File I/O ---------- */
static void saveData(void) {
    FILE *f = fopen(DATA_FILE, "w");
    if (!f) { printf("Could not save data file.\n"); return; }
    for (int i = 0; i < count; i++)
        fprintf(f, "%d|%d|%d|%d|%s\n", items[i].id, items[i].cat,
                items[i].qty, items[i].min, items[i].name);
    fclose(f);
}

static void addItem(const char *name, int cat, int qty, int min) {
    if (count >= MAX_ITEMS) return;
    Product *p = &items[count++];
    p->id = nextId++;
    p->cat = cat; p->qty = qty; p->min = min;
    snprintf(p->name, NAME_LEN, "%s", name);
}

static void loadSample(void) {
    addItem("Intel Core i5-14400F", 0, 14, 5);
    addItem("AMD Ryzen 5 5600", 0, 0, 5);
    addItem("ASUS PRIME B760M", 1, 8, 3);
    addItem("Kingston Fury DDR5 16GB", 2, 25, 10);
    addItem("Corsair Vengeance DDR4 8GB", 2, 6, 8);
    addItem("NVIDIA RTX 4060 8GB", 3, 5, 3);
    addItem("Samsung 980 NVMe 1TB", 4, 18, 6);
    addItem("Corsair RM750e 750W", 5, 7, 3);
    addItem("LG UltraGear 27-inch", 6, 3, 3);
    addItem("Razer DeathAdder", 7, 1, 5);
}

static void loadData(void) {
    FILE *f = fopen(DATA_FILE, "r");
    if (!f) { loadSample(); saveData(); return; }
    char line[256];
    while (count < MAX_ITEMS && fgets(line, sizeof line, f)) {
        Product p;
        char name[NAME_LEN];
        if (sscanf(line, "%d|%d|%d|%d|%59[^\r\n]", &p.id, &p.cat, &p.qty, &p.min, name) == 5
            && p.cat >= 0 && p.cat < NUM_CATS) {
            snprintf(p.name, NAME_LEN, "%s", name);
            items[count++] = p;
            if (p.id >= nextId) nextId = p.id + 1;
        }
    }
    fclose(f);
}

/* ---------- Display ---------- */
static void printHeader(void) {
    printf("\n%-4s %-28s %-14s %6s %6s  %s\n", "ID", "Product", "Category", "Qty", "Min", "Status");
    printf("--------------------------------------------------------------------------\n");
}

static void printRow(const Product *p) {
    printf("%-4d %-28s %-14s %6d %6d  %s\n", p->id, p->name, CATS[p->cat], p->qty, p->min, statusText(p));
}

static void showAlerts(void) {
    int out = 0, low = 0;
    for (int i = 0; i < count; i++) {
        if (items[i].qty <= 0) out++;
        else if (items[i].qty <= items[i].min) low++;
    }
    if (!out && !low) { printf("\nNo stock alerts.\n"); return; }
    printf("\n!!!!!!!!!!!!!!!! STOCK ALERTS !!!!!!!!!!!!!!!!\n");
    if (out) {
        printf("Out of stock (%d):\n", out);
        for (int i = 0; i < count; i++)
            if (items[i].qty <= 0) printf("  - %s (%s)\n", items[i].name, CATS[items[i].cat]);
    }
    if (low) {
        printf("Running low (%d):\n", low);
        for (int i = 0; i < count; i++)
            if (items[i].qty > 0 && items[i].qty <= items[i].min)
                printf("  - %s (%s): %d left\n", items[i].name, CATS[items[i].cat], items[i].qty);
    }
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

static void showAll(void) {
    if (!count) { printf("\nNo products yet.\n"); return; }
    printHeader();
    for (int i = 0; i < count; i++) printRow(&items[i]);
}

static void showByCategory(void) {
    for (int c = 0; c < NUM_CATS; c++) {
        int n = 0, warn = 0;
        for (int i = 0; i < count; i++)
            if (items[i].cat == c) { n++; if (items[i].qty <= items[i].min) warn++; }
        printf("\n=== Category: %s (%d items, %d need attention) ===", CATS[c], n, warn);
        if (!n) { printf("\n  (no products)\n"); continue; }
        printHeader();
        for (int i = 0; i < count; i++) if (items[i].cat == c) printRow(&items[i]);
    }
}

/* ---------- Actions ---------- */
static void actionAdd(void) {
    char name[NAME_LEN];
    if (count >= MAX_ITEMS) { printf("Inventory is full.\n"); return; }
    readLine("Product name: ", name, sizeof name);
    if (!name[0] || strchr(name, '|')) { printf("Invalid name (must not be empty or contain '|').\n"); return; }
    int cat = pickCategory();
    int qty = readInt("Initial quantity: ", 0, 1000000);
    int min = readInt("Low-stock threshold: ", 0, 1000000);
    addItem(name, cat, qty, min);
    saveData();
    printf("Product added.\n");
    if (qty <= min) showAlerts();
}

static void actionMove(int receive) {
    showAll();
    if (!count) return;
    int id = readInt("Enter product ID: ", 1, 1000000);
    int idx = findById(id);
    if (idx < 0) { printf("Product ID %d not found.\n", id); return; }
    Product *p = &items[idx];
    int n = readInt(receive ? "Quantity to receive: " : "Quantity to issue: ", 1, 1000000);
    int before = p->qty;
    if (!receive && n > p->qty) {
        printf("Cannot issue %d: only %d of \"%s\" in stock.\n", n, p->qty, p->name);
        return;
    }
    p->qty += receive ? n : -n;
    saveData();
    printf("%s %d x %s. In stock: %d\n", receive ? "Received" : "Issued", n, p->name, p->qty);
    if (p->qty <= 0)
        printf("\n*** ALERT: \"%s\" is now OUT OF STOCK! ***\n", p->name);
    else if (p->qty <= p->min && before > p->min)
        printf("\n*** ALERT: \"%s\" is running low (%d left). ***\n", p->name, p->qty);
}

static void actionSetMin(void) {
    showAll();
    if (!count) return;
    int idx = findById(readInt("Enter product ID: ", 1, 1000000));
    if (idx < 0) { printf("Product not found.\n"); return; }
    items[idx].min = readInt("New low-stock threshold: ", 0, 1000000);
    saveData();
    printf("Threshold updated.\n");
}

static void actionDelete(void) {
    showAll();
    if (!count) return;
    int idx = findById(readInt("Enter product ID to delete: ", 1, 1000000));
    if (idx < 0) { printf("Product not found.\n"); return; }
    char ans[8];
    printf("Delete \"%s\"?\n", items[idx].name);
    readLine("Type y to confirm: ", ans, sizeof ans);
    if (ans[0] != 'y' && ans[0] != 'Y') { printf("Cancelled.\n"); return; }
    items[idx] = items[count - 1];
    count--;
    saveData();
    printf("Deleted.\n");
}

int main(void) {
    loadData();
    printf("===== Computer Parts Inventory System =====");
    showAlerts();
    for (;;) {
        printf("\n--------- Menu ---------\n"
               "1) Show all products\n"
               "2) Show by category\n"
               "3) Add new product\n"
               "4) Receive stock\n"
               "5) Issue stock\n"
               "6) View alerts (low / out of stock)\n"
               "7) Change low-stock threshold\n"
               "8) Delete product\n"
               "0) Exit\n");
        switch (readInt("Choose: ", 0, 8)) {
            case 1: showAll(); break;
            case 2: showByCategory(); break;
            case 3: actionAdd(); break;
            case 4: actionMove(1); break;
            case 5: actionMove(0); break;
            case 6: showAlerts(); break;
            case 7: actionSetMin(); break;
            case 8: actionDelete(); break;
            case 0: saveData(); printf("Goodbye.\n"); return 0;
        }
    }
}
