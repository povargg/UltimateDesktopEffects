# Architektur – UltimateDesktopEffects 1.0.0

## Ein gemeinsamer Host

`main.cpp` besitzt genau einen Programmeinstieg, eine Prozesssperre, ein Nachrichtenfenster, ein Tray-Symbol und ein transparentes Overlay. `world.cpp` erfasst Monitore, DPI, Fenster, Vordergrundreihenfolge und Vollbildstatus. Es wird für den aktuell gewählten Effekt verwendet. Windows-Ereignisse lösen Aktualisierungen aus; ein ergänzender Abgleich erfolgt viermal pro Sekunde.

Die vier ursprünglichen Effektmodule sind statisch in dieselbe EXE kompiliert. Es werden weder die Einzel-EXEs eingebettet noch zusätzliche Effektprozesse gestartet.

| Modul | Namensraum | Berechnung | Zeichnung |
| --- | --- | --- | --- |
| Schnee | `snowfx` | `snow.cpp`, `snow_piles.cpp` | ursprünglicher Schneerenderer |
| Regen | `rainfx` | `rain.cpp`, `rain_water.cpp` | ursprünglicher Regenrenderer einschließlich Blitzen |
| Blätter | `leavesfx` | `leaves.cpp`, `leaf_piles.cpp` | korrigierter Pixelrenderer mit Stapelcache |
| Halloween | `halloweenfx` | `scene.cpp`, `piles.cpp`, `spiders.cpp` | Pixelrenderer, Stapelcache, Fäden und Spinnen |

Die neun Berechnungsdateien besitzen unveränderte Funktionskörper. Geändert wurden ihre Einbindung und der Namensraum; die Schneeklasse erhielt außerdem einen lesenden Zeit-Getter für gemeinsame Prüfungen. Der Original-Schneerenderer verwendet nun den für `CreateDXGIFactory2` erforderlichen Header `dxgi1_3.h` und passende HRESULT-Vergleiche für die MinGW-Kompilierung. Die Oberfläche erhielt den gemeinsamen Markennamen, beide Autoren, fünf Sprachen und die Effektauswahl.

## Effektwechsel

Die Auswahl schickt `WM_APP_EFFECT` an den Host. Dadurch erfolgt der Wechsel erst nach Rückkehr aus dem Widget-Callback. Der Host entfernt den Maus-Hook, schließt die bisherigen Panels, wählt die neue CPU-Szene und gleicht sie mit dem aktuellen Desktop ab. Die bisherigen GPU-Ressourcen werden freigegeben. Der nächste Frame erstellt ausschließlich die Darstellung des neu gewählten Effekts. Ein Versions- beziehungsweise Auswahlvergleich verhindert, dass mit dem falschen Renderer gezeichnet wird.

`Effects::Update` prüft Aktivierung, Vollbildpause und den Zeitschritt. Es ruft danach genau einen der vier Effekte auf. Null, negative oder nicht endliche Zeitschritte werden verworfen; lange Schritte werden auf 0,1 Sekunden begrenzt. Nicht ausgewählte Szenen erhalten keine Updates. Beim Zurückwechseln werden ihre Fenster und Monitore neu abgeglichen, bevor sie fortgesetzt werden.

Die CPU-Zustände bleiben während der Laufzeit erhalten. Dadurch gehen beim Umschalten weder angesammelte Motive noch Spinnenzustände verloren, soweit die zugehörigen Fenster beziehungsweise Monitore noch bestehen. Geschlossene oder geänderte Fenster werden entsprechend der ursprünglichen Regeln behandelt. Die Zustände werden beim Programmende nicht auf die Festplatte geschrieben; nur die Einstellungen bleiben erhalten.

## Profile und Dateispeicherung

`settings_api.h` definiert die gemeinsame Auswahl und Sprache. Die ursprünglichen vier Settings-Strukturen bleiben getrennt:

| INI-Sektion | Anzahl Felder |
| --- | --- |
| `snow` | 22 |
| `rain` | 31 |
| `leaves` | 27 |
| `halloween` | 34 |

`config.cpp` kodiert die 114 Profilfelder und die beiden gemeinsamen Felder. Die Sprache wird über alle Profile synchronisiert. Originale Grenzwerte begrenzen geladene Zahlen. Unbekannte Schlüssel und ungültige Zahlen werden ignoriert. Beim ersten Start werden verfügbare alte Effektsektionen eingelesen. Beim späteren Speichern wird die gesamte neue Konfiguration über eine temporäre Datei und atomaren Austausch ersetzt; die letzte gültige Datei wird bei Schreibfehlern erhalten.

Autostart, Installation, Prozesssperre und Konfigurationsordner verwenden ausschließlich `UltimateDesktopEffects` als neue Identität. Die Einzelversionen werden dabei nicht verändert.

## Oberfläche und Pixelgrafiken

Die ursprünglichen vier Panel-Klassen bleiben mit ihren jeweiligen Motiven erhalten. Ein gemeinsamer Template-Helfer ergänzt die Effektauswahl. Eine einheitliche Verschiebung aller Widgetrechtecke sowie eine angepasste Fensterhöhe erhalten die bestehenden Bedienelemente. Eigene Fensterklassen je Modul verhindern Konflikte untereinander und mit separat installierten Versionen.

Alle fünf Sprachen haben vollständig belegte Texttabellen: 72 Zeilen für Schnee, 88 für Regen, 83 für Blätter, 94 für Halloween und 99 für den gemeinsamen Host. Die Auswahl der Sprache gilt sofort für die Anwendung. Im Header verlinken zwei Autorenzeilen auf povargg und die ursprüngliche LayerSnow-Arbeit von flipkips. Installer, Dateiinformationen und Lizenzdialog enthalten beide Namen.

Die zwölf Blatt- und zehn Halloween-Sprites bleiben unverändert: 16×16 Pixel, harte Transparenz und begrenzte Palette. Die tatsächlichen PNG-Pixel, die C++-Arrays und die eingebetteten EXE-Daten werden verglichen. Gesammelte Motive besitzen volle Höhe, kleine Kollisionsflächen und überlappen entsprechend der korrigierten Blätterversion.

## Maus und Sichtbarkeit

Nur der aktive Halloween-Effekt kann einen Thread-Hit auswerten. Ein bedarfsgesteuerter `WH_MOUSE_LL`-Hook leitet nicht verarbeitete Ereignisse weiter. Eigene Einstellungen sind ausgeschlossen. Die Treffertoleranz folgt der DPI. In „Hinter Fenstern“ benutzen Zeichnung und Trefferlogik dieselben sichtbaren Linienabschnitte. Bei einem erfolgreichen Schnitt werden der linke Tastendruck und das zugehörige Loslassen verbraucht. Beim Effektwechsel oder bei einer Pause wird der Hook entfernt.

## API-Referenzen

- [Microsoft: CreateDXGIFactory2](https://learn.microsoft.com/en-us/windows/win32/api/dxgi1_3/nf-dxgi1_3-createdxgifactory2): benötigter Header und Factory-Schnittstelle.
- [Microsoft: CreateTargetForHwnd](https://learn.microsoft.com/en-us/windows/win32/api/dcomp/nf-dcomp-idcompositiondevice-createtargetforhwnd): ein bereits belegter Composition-Layer wird nach Zerstörung des alten Targets für das neue Target frei.
- [Microsoft: LowLevelMouseProc](https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelmouseproc): Weiterleitung nicht verarbeiteter Mausereignisse.

Die API-Prüfung und erfolgreiche Kompilierung ersetzen keinen praktischen Windows-GPU- und Eingabetest. Dessen Grenzen sind im Validierungsbericht ausdrücklich ausgewiesen.
