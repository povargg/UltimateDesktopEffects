# UltimateDesktopEffects 1.0.1

Kleines Update des am 1. November 2026 geprüften Basisstands 1.0.0.

## Änderungen

- Im Blättermodus lautet `S_ACCUMULATION` auf Russisch jetzt genau **Накопление Листьев** und auf Ukrainisch **Накопичення Листя**. Die übrigen drei Sprachwerte und alle anderen Texte dieser Tabelle bleiben gleich.
- Neues Pixel-Icon: **UE** mit einer gemeinsamen weißen Kochmütze, deren Band die Oberkanten beider Buchstaben überdeckt. Native Auflösung 32×32, begrenzte Palette und harte Transparenz.
- Windows-Icongrößen: 16, 24, 32, 48, 64, 128 und 256 Pixel. Die Vergrößerungen verwenden Nearest-Neighbor-Interpolation.
- Programm- und Paketversion auf 1.0.1 erhöht.

Die neue Grafik wurde mit der integrierten Bildgenerierung erstellt. Unverändertes Generierungsergebnis: `res/icon/ue-chef-generated.png`. Native, für Windows vorbereitete Fassung: `res/icon/ue-chef-32.png`. Exakter Prompt und Erzeugungsmodus: `res/icon/generation.json`. Die Windows-Datei `res/ultimate.ico` lässt sich mit `python3 tools/make_icon.py` aus diesen vorhandenen Grafiken erneut erzeugen. Eine neue Bildgenerierung ist zum Bauen nicht notwendig.

## Gezielte Prüfung

Die bereits bestandenen 185 Simulationstests des Basisstands wurden für diese Text-/Iconkorrektur nicht erneut ausgeführt. Bewegungs-, Kollisions-, Spinnen- und Umschaltlogik sind unverändert. Die Ausgangsdateien wurden zusätzlich mit dem tatsächlich ausgelieferten Quellcodearchiv 1.0.0 verglichen.

Für das Update wurden die 15 vorhandenen Grafik-, Integrations- und EXE-Paketprüfungen erneut ausgeführt. Zusätzliche gezielte Kontrollen prüfen die beiden exakten Beschriftungen im Quelltext und in der kompilierten EXE, sämtliche Icongrößen und ihre tatsächlich eingebetteten Bilddaten sowie die Begrenzung der Quellcodeänderungen auf Texte, Icon und Versionsangaben. Die 16-, 24-, 32- und 48-Pixel-Varianten wurden auf hellem und dunklem Hintergrund visuell geprüft.

Ergebnis und Buildausgabe: [patch-1.0.1-testprotokoll.txt](patch-1.0.1-testprotokoll.txt).

Die Grenzen der Windows-GUI-Prüfung bleiben wie im [Basisbericht](VALIDIERUNG.md): In dieser Linux-Umgebung wurde die native Windows-Oberfläche nicht gestartet. Das Programm ist nicht digital signiert.
