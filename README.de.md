# UltimateDesktopEffects 1.0.1

**Von [povargg](https://github.com/povargg) und [flipkips](https://github.com/flipkips/desktop-snow-overlay).** Die Windows-Grundlage und der Schnee stammen aus LayerSnow von flipkips; Regen, Blätter, Halloween und die gemeinsame Anwendung bauen darauf auf. MIT-Lizenz.

Eine Windows-App mit vier direkt umschaltbaren Effekten: **Schnee, Regen, Blätter und Halloween**. Alle bisherigen Effektparameter bleiben verfügbar. Sprache, Installation, Tray-Symbol und Windows-Autostart werden gemeinsam verwaltet.

## Änderung in 1.0.1

Die Beschriftung zur Ansammlung im Blättermodus lautet jetzt auf Russisch **Накопление Листьев** und auf Ukrainisch **Накопичення Листя**. Das neue Pixel-Icon zeigt **UE** mit einer gemeinsamen Kochmütze über den Oberkanten beider Buchstaben.

Gezielte Prüfungen dieses kleinen Updates: [docs/PATCH-1.0.1.md](docs/PATCH-1.0.1.md). Die 185 Haupttests stammen unverändert vom geprüften Basisstand 1.0.0.

## Herunterladen und starten

| Datei | Verwendung |
| --- | --- |
| `UltimateDesktopEffects.exe` | Doppelklick öffnet die einmalige Installation. Danach über das Startmenü oder die installierte EXE starten. |
| `UltimateDesktopEffects-Portable.exe` | Per Doppelklick sofort starten. Der Dateiname muss `Portable` enthalten. |
| `UltimateDesktopEffects-Quellcode.zip` | Vollständiger C++17-Quellcode, Pixelgrafiken, Tests, Buildskripte und Dokumentation. |

Windows 10 ab Version 1703 und Windows 11, **64 Bit**, mit Direct3D-11-Unterstützung. Es werden keine zusätzlichen .NET-, Python- oder Visual-C++-Laufzeitpakete zum Ausführen benötigt.

Die Installation erfolgt benutzerbezogen nach `%LOCALAPPDATA%\Programs\UltimateDesktopEffects`, ohne Administratorrechte. Optional können eine Desktop-Verknüpfung und ein Windows-Autostart eingerichtet werden. Einstellungen liegen in `%APPDATA%\UltimateDesktopEffects\settings.ini`, auch bei portablem Start.

Das Symbol im Infobereich öffnet mit **Linksklick** die Einstellungen, mit **Rechtsklick** das Schnellmenü. Eine zweite Instanz öffnet die Einstellungen der bereits laufenden App.

## Zwischen den vier Effekten wechseln

Die gemeinsame Auswahl **Schnee / Regen / Blätter / Halloween** befindet sich oben im Hauptfenster, in den zusätzlichen Einstellungsfenstern und im Schnellmenü. Beim Wechsel werden die passenden Einstellungen angezeigt. Eine weitere Installation ist dafür nicht erforderlich.

Jeder Effekt besitzt ein eigenes Profil für Dichte, Geschwindigkeit, Wind, Ansammlung, Bildrate und seine besonderen Einstellungen. Die laufende Szene des bisherigen Effekts pausiert einschließlich Ansammlungen und wird bei der Rückkehr fortgesetzt. Nur die ausgewählte Szene wird berechnet und gezeichnet. Beim Beenden werden die Einstellungen und die letzte Effektauswahl gespeichert; nach einem Programmneustart bauen sich die Ansammlungen neu auf, wie bei den Einzelversionen.

| Effekt | Erhaltene Funktionen |
| --- | --- |
| Schnee, ursprünglich LayerSnow / „SnowLayer“ | Weiche Flocken und vier Kristallformen, vier Schnee-Voreinstellungen, Wind, Schneeverwehungen, Schmelzen, Abrutschen und Abschütteln |
| Regen | Tropfen, große Tropfen, Spritzer, Wasserflächen und Wellen, Verdunstung, vier Regen-Voreinstellungen, optionale Blitze und optionaler Bildschirmblitz |
| Blätter | Sakura, grüne Frühlings-/Sommerblätter und Herbstblätter, je vier Pixelvarianten, Schwingen, Taumeln und die korrigierten dichten Ansammlungen mit voller Motivfläche |
| Halloween | Sechs Pixel-Süßigkeiten und vier Spinnentypen, Süßigkeiten / Spinnen / Beides, langsames Abseilen, teilweiser Abstieg, Hängepause, 50/50-Hochklettern oder Fallenlassen und abschneidbare Fäden |

Die bisherigen Funktionen für vor/hinter Fenstern, Fenster- und Taskleistenansammlungen, Mauswind, Wegfegen mit dem Cursor, Abschütteln, Vollbildpause, Deckkraft und Bildrate bleiben beim jeweiligen Effekt erhalten. Nur Halloween wertet Fadentreffer aus. Unsichtbare oder durch Vordergrundfenster verdeckte Fäden sind nicht anklickbar. Normale Klicks werden weitergereicht; ein erfolgreicher Schnitt verbraucht den betreffenden Linksklick.

## Sprache und vorhandene Einstellungen

Alle Oberflächen bieten **Deutsch, Englisch, Russisch, Ukrainisch und Rumänisch**. Die Sprache gilt für die gesamte Anwendung; auch der Schneemodus besitzt jetzt Deutsch als fünfte Sprache. Beide Autoren stehen im Interface und Installer.

Beim ersten Start werden vorhandene Effektparameter aus den bisherigen Ordnern `LayerSnow` beziehungsweise `SnowLayer`, `LayerRain`, `LayerLeaves` und `LayerHalloween` gelesen, sofern noch keine neue Konfigurationsdatei besteht. Die neuen Profile werden anschließend gemeinsam gespeichert. Bereits vorhandene Einzelversionen und deren Dateien bleiben eigenständig. Für die gemeinsame Anwendung genügt es, UltimateDesktopEffects zu starten; frühere Einzelversionen können separat beendet oder deinstalliert werden.

## Leistung

Es gibt einen Prozess, ein Overlay, eine Desktop-Erfassung und einen aktiven Effekt. Die ursprünglichen neun Dateien mit Simulationslogik wurden unverändert in eigene C++-Namensräume übernommen. Inaktive Effekte behalten ihren begrenzten CPU-Zustand, führen aber keine Simulationsschritte aus. Beim Wechsel werden die GPU-Ressourcen der bisherigen Darstellung und die geschlossenen Einstellungsfenster freigegeben. Die ursprünglichen Grenzen für Partikel, Stapel und Spinnen sowie die reduzierte Bildrate bei statischen Ansammlungen bleiben erhalten.

Ein nativer CPU-Vergleich über jeweils 3.600 Simulationsschritte lag für die gemeinsame Steuerung bei etwa **0,96–1,03 ×** dem Aufwand direkter Aufrufe derselben Routinen. Das ist ein Linux-Simulationstest, kein Windows-GPU-Benchmark und keine allgemeine Prozentangabe zur CPU-Auslastung. Details: [docs/VALIDIERUNG.md](docs/VALIDIERUNG.md).

## Bauen

### MinGW-w64, auch unter Linux

```sh
python3 build_mingw.py --cxx x86_64-w64-mingw32-g++-posix
```

### Windows mit Visual Studio

Visual Studio 2022 Build Tools mit C++-Desktopentwicklung und Windows SDK installieren, dann `build.bat` ausführen. Dieses alternative Buildskript ist enthalten; der ausgelieferte Build wurde tatsächlich mit MinGW-w64 kompiliert.

Ausgaben: `dist/UltimateDesktopEffects.exe`, `dist/UltimateDesktopEffects-Portable.exe`, `dist/UltimateDesktopEffects-Uninstall.exe`. Alle drei enthalten dieselben Bytes; Dateiname und Startparameter steuern den Betriebsmodus.

| Parameter | Wirkung |
| --- | --- |
| `/portable` | Direkt starten, ohne Installationsdialog |
| `/uninstall` | Deinstallationsdialog |
| `/uninstall /quiet` | Installation und Einstellungen ohne weiteren Dialog entfernen |
| `/autostart` | Von Windows automatisch gestartete Kopie |

Zum Deinstallieren stehen „Installierte Apps“, die Startmenü-Verknüpfung und `uninstall.exe` im Installationsordner zur Verfügung. Das Entfernen der Einstellungen ist im normalen Dialog wählbar.

## Tests und Prüfgrenzen

```sh
sh run-tests.sh
python3 tests/release_checks.py
```

Der native Testlauf benötigt einen C++17-Compiler mit AddressSanitizer und UndefinedBehaviorSanitizer. Die Paketprüfungen benötigen Python 3, Pillow und zuvor kompilierte Windows-EXEs. **Ergebnis des Basisstands 1.0.0: 200 Prüfungen bestanden, 0 fehlgeschlagen.** Davon sind 185 echte C++-Simulationstests und 15 Grafik-, Quellcodeintegrations- und Paketprüfungen. Ausgaben liegen in [docs/testprotokoll.txt](docs/testprotokoll.txt).

Die Windows-EXE wurde erfolgreich ohne Compilerwarnungen erstellt. In der verfügbaren Linux-Umgebung konnten die Windows-Oberfläche, tatsächliche Maus-Hook-Zustellung, Installation und GPU-Komposition nicht praktisch ausgeführt werden. Die EXE ist nicht digital signiert. Die Offline-Vorschauen zeigen echte Simulationsdaten mit vereinfachter Zeichnung und sind keine Windows-Bildschirmaufnahmen.

## Quellcode und Herkunft

- [docs/ARCHITEKTUR.md](docs/ARCHITEKTUR.md): gemeinsamer Host, Effektwechsel, Profile und Ressourcennutzung.
- [docs/VALIDIERUNG.md](docs/VALIDIERUNG.md): Prüffälle, Leistungswerte und Grenzen.
- `docs/source-provenance.json`: Ausgangsarchive, ursprünglicher Git-Commit und Vergleichswerte der Simulationsdateien.
- `res/leaves` und `res/halloween`: alle Pixelgrafiken einschließlich Generierungsvorlagen und Prompts.
- [LICENSE](LICENSE) und `licenses/`: MIT sowie Laufzeithinweise.

Die Anwendung arbeitet lokal. Es gibt keine Konten, Telemetrie oder Downloads von Effekten zur Laufzeit. Die GitHub-Links werden nur auf ausdrücklichen Klick im Browser geöffnet.
