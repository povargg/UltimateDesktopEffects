#include "i18n.h"
#include "../../settings_api.h"
namespace leavesfx {
const wchar_t* const kLangCodes[LANG_COUNT]={L"EN",L"RU",L"UA",L"RO",L"DE"};
static const wchar_t* const kText[][LANG_COUNT]={
 // S_MODE_TOP
 {L"Over windows", L"Поверх окон", L"Поверх вікон", L"Peste ferestre", L"Vor Fenstern"},
 // S_MODE_BEHIND
 {L"Behind windows", L"За окнами", L"За вікнами", L"Sub ferestre", L"Hinter Fenstern"},
 // S_INTENSITY
 {L"Leaf density", L"Количество листьев", L"Кількість листя", L"Densitatea frunzelor", L"Blätterdichte"},
 // S_FALL_SPEED
 {L"Fall speed", L"Скорость падения", L"Швидкість падіння", L"Viteza căderii", L"Fallgeschwindigkeit"},
 // S_WIND_ANGLE
 {L"Wind angle", L"Угол ветра", L"Кут вітру", L"Unghiul vântului", L"Windwinkel"},
 // S_GUSTS
 {L"Wind gusts", L"Порывы ветра", L"Пориви вітру", L"Rafale de vânt", L"Windböen"},
 // S_CURSOR_WIND
 {L"Wind follows cursor", L"Ветер за курсором", L"Вітер за курсором", L"Vântul după cursor", L"Wind folgt der Maus"},
 // S_ACCUMULATION
 {L"Accumulation", L"Накопление Листьев", L"Накопичення Листя", L"Acumulare", L"Ansammlung"},
 // S_CURSOR_RADIUS
 {L"Cursor radius", L"Радиус курсора", L"Радіус курсора", L"Raza cursorului", L"Mausradius"},
 // S_SMOOTHNESS
 {L"Smoothness", L"Плавность", L"Плавність", L"Fluiditate", L"Bildrate"},
 // S_CARD_BEHAVIOR
 {L"Behavior", L"Поведение", L"Поведінка", L"Comportament", L"Verhalten"},
 // S_PAUSE_GAMES
 {L"Hide in games and fullscreen apps", L"Скрывать в играх и полноэкранных приложениях", L"Ховати в іграх і повноекранних програмах", L"Ascunde în jocuri și aplicații pe tot ecranul", L"In Spielen / Vollbild pausieren"},
 // S_AUTOSTART
 {L"Start with Windows", L"Запускать вместе с Windows", L"Запускати разом з Windows", L"Pornește odată cu Windows", L"Mit Windows starten"},
 // S_LANGUAGE
 {L"Language", L"Язык", L"Мова", L"Limbă", L"Sprache"},
 // S_DEFAULTS
 {L"Defaults", L"По умолчанию", L"За замовчуванням", L"Implicit", L"Standardwerte"},
 // S_DONE
 {L"Done", L"Готово", L"Готово", L"Gata", L"Fertig"},
 // S_MODE
 {L"Mode", L"Режим", L"Режим", L"Mod", L"Anzeigemodus"},
 // S_SMOOTHNESS_FPS
 {L"Smoothness, FPS", L"Плавность, FPS", L"Плавність, FPS", L"Fluiditate, FPS", L"Bildrate (FPS)"},
 // S_ALL_SETTINGS
 {L"All settings…", L"Все настройки…", L"Усі налаштування…", L"Toate setările…", L"Alle Einstellungen…"},
 // S_EXIT
 {L"Exit", L"Выход", L"Вихід", L"Ieșire", L"Beenden"},
 // S_WHAT_INSTALLED
 {L"What gets installed", L"Что будет установлено", L"Що буде встановлено", L"Ce se instalează", L"Was installiert wird"},
 // S_FOLDER
 {L"Folder: ", L"Папка: ", L"Папка: ", L"Dosar: ", L"Ordner: "},
 // S_OPTIONS
 {L"Options", L"Параметры", L"Параметри", L"Opțiuni", L"Optionen"},
 // S_DESKTOP_LINK
 {L"Desktop shortcut", L"Ярлык на рабочем столе", L"Ярлик на робочому столі", L"Comandă rapidă pe desktop", L"Desktop-Verknüpfung"},
 // S_RUN_AFTER
 {L"Launch after install", L"Запустить после установки", L"Запустити після встановлення", L"Pornește după instalare", L"Nach der Installation starten"},
 // S_CANCEL
 {L"Cancel", L"Отмена", L"Скасувати", L"Anulează", L"Abbrechen"},
 // S_INSTALL
 {L"Install", L"Установить", L"Встановити", L"Instalează", L"Installieren"},
 // S_UPDATE
 {L"Update", L"Обновить", L"Оновити", L"Actualizează", L"Aktualisieren"},
 // S_COPY_FAILED
 {L"Could not copy the file to\n", L"Не удалось скопировать файл в папку\n", L"Не вдалося скопіювати файл у папку\n", L"Fișierul nu a putut fi copiat în\n", L"Datei konnte nicht kopiert werden nach\n"},
 // S_INSTALL_ERROR
 {L"Installation error", L"Ошибка установки", L"Помилка встановлення", L"Eroare la instalare", L"Installationsfehler"},
 // S_UNINSTALL_CARD
 {L"What will be removed", L"Что будет удалено", L"Що буде видалено", L"Ce se elimină", L"Was entfernt wird"},
 // S_UNINSTALL_DESC
 {L"The program, its Start menu and desktop shortcuts, and autostart.", L"Программа, её ярлыки в меню «Пуск» и на рабочем столе, автозапуск.", L"Програма, її ярлики в меню «Пуск» і на робочому столі, автозапуск.", L"Programul, comenzile rapide din meniul Start și de pe desktop, pornirea automată.", L"Die Anwendung, Verknüpfungen und der Autostart-Eintrag."},
 // S_DELETE_SETTINGS
 {L"Also delete my settings", L"Удалить и мои настройки", L"Видалити й мої налаштування", L"Șterge și setările mele", L"Auch meine Einstellungen löschen"},
 // S_UNINSTALL
 {L"Uninstall", L"Удалить", L"Видалити", L"Dezinstalează", L"Deinstallieren"},
 // S_CLOSE
 {L"Close", L"Закрыть", L"Закрити", L"Închide", L"Schließen"},
 // S_LICENSE
 {L"Credits / MIT license", L"Авторы / лицензия MIT", L"Автори / ліцензія MIT", L"Autori / licență MIT", L"Mitwirkende / MIT-Lizenz"},
 // S_TRAY_TIP
 {L"UltimateDesktopEffects: pixel leaves by povargg", L"UltimateDesktopEffects: пиксельные листья от povargg", L"UltimateDesktopEffects: піксельне листя від povargg", L"UltimateDesktopEffects: frunze pixel de povargg", L"UltimateDesktopEffects: Pixelblätter von povargg"},
 // S_WELCOME_TITLE
 {L"UltimateDesktopEffects is running", L"UltimateDesktopEffects запущен", L"UltimateDesktopEffects запущено", L"UltimateDesktopEffects rulează", L"UltimateDesktopEffects läuft"},
 // S_WELCOME_TEXT
 {L"The leaf icon is in the tray. Left click: settings. Right click: leaf type and quick menu.", L"Значок листа в трее. Левый клик: настройки. Правый: выбор листьев и меню.", L"Значок листка в треї. Лівий клік: налаштування. Правий: вибір листя та меню.", L"Frunza este în zona de notificare. Clic stânga: setări. Clic dreapta: tipul frunzei și meniu.", L"Das Blattsymbol ist im Infobereich. Linksklick: Einstellungen. Rechtsklick: Blattart und Schnellmenü."},
 // S_SETTINGS_TITLE
 {L"UltimateDesktopEffects: settings", L"UltimateDesktopEffects: настройки", L"UltimateDesktopEffects: налаштування", L"UltimateDesktopEffects: setări", L"UltimateDesktopEffects: Einstellungen"},
 // S_SUBTITLE
 {L"Pixel leaves on your desktop", L"Пиксельные листья на рабочем столе", L"Піксельне листя на робочому столі", L"Frunze pixel pe desktop", L"Pixelblätter auf deinem Desktop"},
 // S_PROJECT_BY
 {L"A project by povargg", L"Проект povargg", L"Проєкт povargg", L"Un proiect de povargg", L"Ein Projekt von povargg"},
 // S_AUTHOR
 {L"Based on LayerSnow by flipkips", L"Основа: LayerSnow от flipkips", L"Основа: LayerSnow від flipkips", L"Bază: LayerSnow de flipkips", L"Basis: LayerSnow von flipkips"},
 // S_STAR_GITHUB
 {L"povargg on GitHub", L"povargg на GitHub", L"povargg на GitHub", L"povargg pe GitHub", L"povargg auf GitHub"},
 // S_CARD_LEAVES
 {L"Falling leaves", L"Листопад", L"Листопад", L"Frunze în cădere", L"Fallende Blätter"},
 // S_LEAF_TYPE
 {L"Leaf type", L"Вид листьев", L"Тип листя", L"Tip de frunză", L"Blattart"},
 // S_TYPE_SAKURA
 {L"Sakura", L"Сакура", L"Сакура", L"Sakura", L"Sakura"},
 // S_TYPE_SUMMER
 {L"Summer", L"Лето", L"Літо", L"Vară", L"Sommer"},
 // S_TYPE_AUTUMN
 {L"Autumn", L"Осень", L"Осінь", L"Toamnă", L"Herbst"},
 // S_LEAF_SIZE
 {L"Leaf size", L"Размер листьев", L"Розмір листя", L"Mărimea frunzelor", L"Blattgröße"},
 // S_VARIETY
 {L"Variant diversity", L"Разнообразие", L"Різноманітність", L"Diversitate", L"Variantenvielfalt"},
 // S_CARD_PILES
 {L"Leaf layers", L"Слои листьев", L"Шари листя", L"Straturi de frunze", L"Laubschichten"},
 // S_PILE_TASKBAR
 {L"Taskbar layer height", L"Слой над панелью", L"Шар над панеллю", L"Strat pe bara de activități", L"Laubhöhe: Taskleiste"},
 // S_PILE_WINDOWS
 {L"Window layer height", L"Слой на окнах", L"Шар на вікнах", L"Strat pe ferestre", L"Laubhöhe: Fenster"},
 // S_ROUGHNESS
 {L"Layer unevenness", L"Неровность слоя", L"Нерівність шару", L"Neregularitate", L"Unebenheit"},
 // S_DECAY
 {L"Leaf decay", L"Исчезновение листьев", L"Зникнення листя", L"Dispariția frunzelor", L"Laubabbau"},
 // S_LEAVES_ON
 {L"Leaves enabled", L"Листья включены", L"Листя ввімкнено", L"Frunze activate", L"Blätter einschalten"},
 // S_SHOW_FALLING
 {L"Show falling leaves", L"Показывать листопад", L"Показувати листопад", L"Arată frunzele care cad", L"Fallende Blätter anzeigen"},
 // S_ON_WINDOWS
 {L"Collect leaves on windows", L"Листья на окнах", L"Листя на вікнах", L"Frunze pe ferestre", L"Laub auf Fenstern"},
 // S_ON_TASKBAR
 {L"Collect leaves on taskbar", L"Листья над панелью", L"Листя над панеллю", L"Frunze pe bara de activități", L"Laub auf der Taskleiste"},
 // S_CURSOR_KNOCK
 {L"Brush leaves with cursor", L"Сметать курсором", L"Змітати курсором", L"Mătură cu cursorul", L"Laub mit Maus wegfegen"},
 // S_SHAKE
 {L"Shake off leaves", L"Стряхнуть листья", L"Струсити листя", L"Scutură frunzele", L"Laub abschütteln"},
 // S_SHAKE_ALL
 {L"Shake off all leaves", L"Стряхнуть все листья", L"Струсити все листя", L"Scutură toate frunzele", L"Alles Laub abschütteln"},
 // S_APPEARANCE
 {L"Leaf appearance…", L"Вид листьев…", L"Вигляд листя…", L"Aspectul frunzelor…", L"Blattdarstellung…"},
 // S_CARD_ANIMATION
 {L"Animation and opacity", L"Анимация и видимость", L"Анімація та видимість", L"Animație și opacitate", L"Animation und Deckkraft"},
 // S_LEAF_OPACITY
 {L"Falling leaf opacity", L"Видимость листопада", L"Видимість листопаду", L"Opacitate frunze", L"Deckkraft: Blätter"},
 // S_PILE_OPACITY
 {L"Layer opacity", L"Видимость слоя", L"Видимість шару", L"Opacitatea stratului", L"Deckkraft: Laubschicht"},
 // S_FLUTTER
 {L"Flutter", L"Покачивание", L"Погойдування", L"Legănare", L"Flattern"},
 // S_TUMBLE
 {L"Tumbling / rotation", L"Вращение", L"Обертання", L"Rotire", L"Drehen und Taumeln"},
 // S_CARD_VARIANTS
 {L"Leaf variants", L"Варианты листьев", L"Варіанти листя", L"Variante de frunze", L"Blattvarianten"},
 // S_LOOK_NOTE
 {L"Changes are live and saved automatically. The artwork is native 16×16 pixels, rendered with crisp nearest-neighbor sampling.", L"Изменения применяются сразу. Графика 16×16 пикселей, без сглаживания.", L"Зміни застосовуються одразу. Графіка 16×16 пікселів, без згладжування.", L"Modificările sunt imediate. Grafică nativă 16×16, cu pixeli clari.", L"Änderungen gelten sofort. Die Grafiken haben echte 16×16 Pixel und werden ohne Weichzeichnung dargestellt."},
 // S_VARIANTS_NOTE
 {L"Four variants per type. Sakura: pink petals. Summer: green leaves. Autumn: yellow, orange, red and copper. Changing type also recolors existing layers.", L"По четыре варианта: розовая сакура, зелёные летние листья, жёлтые, оранжевые, красные и медные осенние. Тип меняется и у лежащих листьев.", L"По чотири варіанти: рожева сакура, зелене літнє листя, жовте, помаранчеве, червоне та мідне осіннє. Тип змінюється і в шарах.", L"Patru variante: petale roz, frunze verzi și frunze de toamnă galbene, portocalii, roșii și arămii. Tipul schimbă și straturile existente.", L"Vier Varianten je Art: rosa Sakura, grüne Sommerblätter sowie gelbes, oranges, rotes und kupferfarbenes Herbstlaub. Der Wechsel gilt auch für vorhandene Schichten."},
 // S_CLEAR_ALL
 {L"Clear all leaves", L"Убрать все листья", L"Прибрати все листя", L"Șterge toate frunzele", L"Alles Laub entfernen"},
 // S_INSTALL_TITLE
 {L"Install UltimateDesktopEffects", L"Установка UltimateDesktopEffects", L"Встановлення UltimateDesktopEffects", L"Instalare UltimateDesktopEffects", L"UltimateDesktopEffects installieren"},
 // S_UPDATE_TITLE
 {L"Update UltimateDesktopEffects", L"Обновление UltimateDesktopEffects", L"Оновлення UltimateDesktopEffects", L"Actualizare UltimateDesktopEffects", L"UltimateDesktopEffects aktualisieren"},
 // S_INSTALL_SUB
 {L"Pixel leaves by povargg", L"Пиксельные листья от povargg", L"Піксельне листя від povargg", L"Frunze pixel de povargg", L"Pixelblätter von povargg"},
 // S_INSTALL_DESC
 {L"A project by povargg. Pixel leaves flutter down and form uneven layers on windows and the taskbar. Sakura, summer and autumn; four variants each. Based on LayerSnow by flipkips (MIT).", L"Проект povargg. Пиксельные листья падают и образуют неровные слои на окнах и панели задач. Сакура, лето, осень, по четыре варианта. Основа: LayerSnow от flipkips (MIT).", L"Проєкт povargg. Піксельне листя падає та утворює нерівні шари на вікнах і панелі завдань. Сакура, літо, осінь, по чотири варіанти. Основа: LayerSnow від flipkips (MIT).", L"Un proiect de povargg. Frunze pixel formează straturi pe ferestre și bara de activități. Sakura, vară, toamnă: patru variante fiecare. Bază: LayerSnow de flipkips (MIT).", L"Ein Projekt von povargg. Pixelblätter fallen und bilden ungleichmäßige Schichten auf Fenstern und Taskleiste. Sakura, Sommer und Herbst mit je vier Varianten. Basis: LayerSnow von flipkips (MIT)."},
 // S_INSTALL_DONE
 {L"Done! Start UltimateDesktopEffects from the Start menu.", L"Готово! Запустите UltimateDesktopEffects из меню «Пуск».", L"Готово! Запустіть UltimateDesktopEffects з меню «Пуск».", L"Gata! Pornește UltimateDesktopEffects din meniul Start.", L"Fertig! UltimateDesktopEffects lässt sich im Startmenü öffnen."},
 // S_UNINSTALL_TITLE
 {L"Uninstall UltimateDesktopEffects", L"Удаление UltimateDesktopEffects", L"Видалення UltimateDesktopEffects", L"Dezinstalare UltimateDesktopEffects", L"UltimateDesktopEffects deinstallieren"},
 // S_UNINSTALL_SUB
 {L"Remove the pixel leaves", L"Убрать пиксельные листья", L"Прибрати піксельне листя", L"Elimină frunzele pixel", L"Pixelblätter entfernen"},
 // S_REMOVED_TITLE
 {L"UltimateDesktopEffects removed", L"UltimateDesktopEffects удалён", L"UltimateDesktopEffects видалено", L"UltimateDesktopEffects a fost eliminat", L"UltimateDesktopEffects wurde entfernt"},
 // S_REMOVED_TEXT
 {L"The leaves have been swept away.", L"Листья убраны.", L"Листя прибрано.", L"Frunzele au fost măturate.", L"Das Laub wurde weggefegt."},
 // S_UNINSTALL_LINK
 {L"Uninstall UltimateDesktopEffects", L"Удалить UltimateDesktopEffects", L"Видалити UltimateDesktopEffects", L"Dezinstalează UltimateDesktopEffects", L"UltimateDesktopEffects deinstallieren"},
};
static_assert(sizeof(kText)/sizeof(kText[0])==S_COUNT,"translation count mismatch");
int CurrentLang(){return ::CurrentLang();}
const wchar_t* Tr(Str id){return kText[id][CurrentLang()];}
}
