#include "i18n.h"
#include "../../settings_api.h"
namespace halloweenfx {
const wchar_t* const kLangCodes[LANG_COUNT]={L"EN",L"RU",L"UA",L"RO",L"DE"};
static const wchar_t* const kText[][LANG_COUNT]={
 // S_MODE_TOP
 {L"Over windows", L"Поверх окон", L"Поверх вікон", L"Peste ferestre", L"Vor Fenstern"},
 // S_MODE_BEHIND
 {L"Behind windows", L"За окнами", L"За вікнами", L"Sub ferestre", L"Hinter Fenstern"},
 // S_INTENSITY
 {L"Density", L"Количество", L"Кількість", L"Densitate", L"Dichte"},
 // S_FALL_SPEED
 {L"Fall speed", L"Скорость падения", L"Швидкість падіння", L"Viteza căderii", L"Fallgeschwindigkeit"},
 // S_WIND_ANGLE
 {L"Wind angle", L"Угол ветра", L"Кут вітру", L"Unghiul vântului", L"Windwinkel"},
 // S_GUSTS
 {L"Wind gusts", L"Порывы ветра", L"Пориви вітру", L"Rafale de vânt", L"Windböen"},
 // S_CURSOR_WIND
 {L"Wind follows cursor", L"Ветер за курсором", L"Вітер за курсором", L"Vântul după cursor", L"Wind folgt der Maus"},
 // S_ACCUMULATION
 {L"Accumulation", L"Накопление", L"Накопичення", L"Acumulare", L"Ansammlung"},
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
 {L"UltimateDesktopEffects: candy and spiders by povargg", L"UltimateDesktopEffects: сладости и пауки от povargg", L"UltimateDesktopEffects: солодощі та павуки від povargg", L"UltimateDesktopEffects: dulciuri și păianjeni de povargg", L"UltimateDesktopEffects: Süßes und Spinnen von povargg"},
 // S_WELCOME_TITLE
 {L"UltimateDesktopEffects is running", L"UltimateDesktopEffects запущен", L"UltimateDesktopEffects запущено", L"UltimateDesktopEffects rulează", L"UltimateDesktopEffects läuft"},
 // S_WELCOME_TEXT
 {L"Look for the pumpkin candy in the tray. Left click: settings. Right click: content mode and quick menu. Click a visible thread to cut it.", L"Значок конфеты в трее. Левый клик: настройки. Правый: режим и меню. Щёлкните по нити, чтобы перерезать её.", L"Значок цукерки в треї. Лівий клік: налаштування. Правий: режим та меню. Натисніть нитку, щоб перерізати її.", L"Bomboana este în zona de notificare. Clic stânga: setări. Clic dreapta: mod și meniu. Taie firul cu un clic.", L"Das Kürbisbonbon ist im Infobereich. Linksklick: Einstellungen. Rechtsklick: Motive und Menü. Ein Klick auf einen sichtbaren Faden schneidet ihn ab."},
 // S_SETTINGS_TITLE
 {L"UltimateDesktopEffects: settings", L"UltimateDesktopEffects: настройки", L"UltimateDesktopEffects: налаштування", L"UltimateDesktopEffects: setări", L"UltimateDesktopEffects: Einstellungen"},
 // S_SUBTITLE
 {L"Pixel treats and spiders on your desktop", L"Сладости и пауки на рабочем столе", L"Солодощі та павуки на робочому столі", L"Dulciuri și păianjeni pe desktop", L"Pixel-Süßigkeiten und Spinnen"},
 // S_PROJECT_BY
 {L"A project by povargg", L"Проект povargg", L"Проєкт povargg", L"Un proiect de povargg", L"Ein Projekt von povargg"},
 // S_AUTHOR
 {L"Based on LayerSnow by flipkips", L"Основа: LayerSnow от flipkips", L"Основа: LayerSnow від flipkips", L"Bază: LayerSnow de flipkips", L"Basis: LayerSnow von flipkips"},
 // S_STAR_GITHUB
 {L"povargg on GitHub", L"povargg на GitHub", L"povargg на GitHub", L"povargg pe GitHub", L"povargg auf GitHub"},
 // S_CARD_LEAVES
 {L"Halloween particles", L"Хеллоуин", L"Гелловін", L"Halloween", L"Halloween-Motive"},
 // S_LEAF_TYPE
 {L"Content mode", L"Режим", L"Режим", L"Mod", L"Motive"},
 // S_LEAF_SIZE
 {L"Sprite size", L"Размер", L"Розмір", L"Mărime", L"Motivgröße"},
 // S_VARIETY
 {L"Variant diversity", L"Разнообразие", L"Різноманітність", L"Diversitate", L"Variantenvielfalt"},
 // S_CARD_PILES
 {L"Collected treats", L"Скопления", L"Скупчення", L"Grămezi", L"Ansammlungen"},
 // S_PILE_TASKBAR
 {L"Taskbar pile height", L"Слой над панелью", L"Шар над панеллю", L"Strat pe bara de activități", L"Höhe: Taskleiste"},
 // S_PILE_WINDOWS
 {L"Window pile height", L"Слой на окнах", L"Шар на вікнах", L"Strat pe ferestre", L"Höhe: Fenster"},
 // S_ROUGHNESS
 {L"Layer unevenness", L"Неровность слоя", L"Нерівність шару", L"Neregularitate", L"Unebenheit"},
 // S_DECAY
 {L"Pile decay", L"Исчезновение", L"Зникнення", L"Dispariție", L"Abbau"},
 // S_LEAVES_ON
 {L"Halloween enabled", L"Хеллоуин включён", L"Гелловін увімкнено", L"Halloween activat", L"Halloween einschalten"},
 // S_SHOW_FALLING
 {L"Show moving sprites", L"Показывать анимацию", L"Показувати анімацію", L"Arată animația", L"Bewegte Motive anzeigen"},
 // S_ON_WINDOWS
 {L"Collect on windows", L"Собирать на окнах", L"Збирати на вікнах", L"Adună pe ferestre", L"Auf Fenstern sammeln"},
 // S_ON_TASKBAR
 {L"Collect on taskbar", L"Собирать над панелью", L"Збирати над панеллю", L"Adună pe bara de activități", L"Auf Taskleiste sammeln"},
 // S_CURSOR_KNOCK
 {L"Brush piles with cursor", L"Сметать курсором", L"Змітати курсором", L"Mătură cu cursorul", L"Mit Maus wegfegen"},
 // S_SHAKE
 {L"Shake off piles", L"Стряхнуть всё", L"Струсити все", L"Scutură grămezile", L"Alles abschütteln"},
 // S_SHAKE_ALL
 {L"Shake off all piles", L"Стряхнуть все скопления", L"Струсити всі скупчення", L"Scutură toate grămezile", L"Alles abschütteln"},
 // S_APPEARANCE
 {L"Spiders and appearance…", L"Пауки и оформление…", L"Павуки та вигляд…", L"Păianjeni și aspect…", L"Spinnen & Darstellung…"},
 // S_CARD_ANIMATION
 {L"Animation and opacity", L"Анимация и видимость", L"Анімація та видимість", L"Animație și opacitate", L"Animation und Deckkraft"},
 // S_LEAF_OPACITY
 {L"Sprite opacity", L"Видимость объектов", L"Видимість об’єктів", L"Opacitate obiecte", L"Deckkraft: Motive"},
 // S_PILE_OPACITY
 {L"Pile opacity", L"Видимость слоя", L"Видимість шару", L"Opacitatea stratului", L"Deckkraft: Schicht"},
 // S_FLUTTER
 {L"Sway", L"Покачивание", L"Погойдування", L"Legănare", L"Seitliches Schwingen"},
 // S_TUMBLE
 {L"Tumbling / rotation", L"Вращение", L"Обертання", L"Rotire", L"Drehen und Taumeln"},
 // S_CARD_VARIANTS
 {L"Pixel sprites", L"Пиксельные мотивы", L"Піксельні мотиви", L"Motive pixel", L"Pixelmotive"},
 // S_LOOK_NOTE
 {L"Native 16×16 sprites and dense full-face piles. Changes are live. Spiders pause, then have a 50% chance to climb or drop.", L"Графика 16×16, плотные слои. После паузы паук с вероятностью 50% поднимается или падает.", L"Графіка 16×16, щільні шари. Після паузи павук із імовірністю 50% піднімається або падає.", L"Grafică 16×16 și grămezi dense. După pauză, păianjenul urcă sau cade cu șanse egale.", L"Echte 16×16-Sprites und dichte Schichten. Nach einer Pause klettern Spinnen mit 50 % Wahrscheinlichkeit hoch oder lassen sich fallen."},
 // S_VARIANTS_NOTE
 {L"Six candy sprites and four spiders. Click a visible thread to cut it.", L"Шесть сладостей и четыре паука. Нить можно перерезать щелчком.", L"Шість солодощів і чотири павуки. Нитку можна перерізати кліком.", L"Șase dulciuri și patru păianjeni. Taie firul vizibil cu un clic.", L"Sechs Süßigkeiten und vier Spinnen. Sichtbare Fäden lassen sich anklicken und abschneiden."},
 // S_CLEAR_ALL
 {L"Clear all", L"Убрать всё", L"Прибрати все", L"Șterge tot", L"Alles entfernen"},
 // S_INSTALL_TITLE
 {L"Install UltimateDesktopEffects", L"Установка UltimateDesktopEffects", L"Встановлення UltimateDesktopEffects", L"Instalare UltimateDesktopEffects", L"UltimateDesktopEffects installieren"},
 // S_UPDATE_TITLE
 {L"Update UltimateDesktopEffects", L"Обновление UltimateDesktopEffects", L"Оновлення UltimateDesktopEffects", L"Actualizare UltimateDesktopEffects", L"UltimateDesktopEffects aktualisieren"},
 // S_INSTALL_SUB
 {L"Halloween pixels by povargg", L"Хеллоуин от povargg", L"Гелловін від povargg", L"Halloween de povargg", L"Halloween-Pixel von povargg"},
 // S_INSTALL_DESC
 {L"A project by povargg. Six pixel candies and four spiders collect on windows and the taskbar. Cut hanging threads with a click. Based on LayerSnow by flipkips (MIT).", L"Проект povargg. Шесть сладостей и четыре паука собираются на окнах и панели задач. Нити можно перерезать. Основа: LayerSnow от flipkips (MIT).", L"Проєкт povargg. Шість солодощів і чотири павуки збираються на вікнах і панелі завдань. Нитки можна перерізати. Основа: LayerSnow від flipkips (MIT).", L"Un proiect de povargg. Șase dulciuri și patru păianjeni pe ferestre și bara de activități. Firele se taie prin clic. Bază: LayerSnow de flipkips (MIT).", L"Ein Projekt von povargg. Sechs Pixel-Süßigkeiten und vier Spinnen sammeln sich auf Fenstern und Taskleiste. Fäden sind per Klick abschneidbar. Basis: LayerSnow von flipkips (MIT)."},
 // S_INSTALL_DONE
 {L"Done! Start UltimateDesktopEffects from the Start menu.", L"Готово! Откройте UltimateDesktopEffects из меню «Пуск».", L"Готово! Відкрийте UltimateDesktopEffects з меню «Пуск».", L"Gata! Pornește UltimateDesktopEffects din meniul Start.", L"Fertig! UltimateDesktopEffects lässt sich im Startmenü öffnen."},
 // S_UNINSTALL_TITLE
 {L"Uninstall UltimateDesktopEffects", L"Удаление UltimateDesktopEffects", L"Видалення UltimateDesktopEffects", L"Dezinstalare UltimateDesktopEffects", L"UltimateDesktopEffects deinstallieren"},
 // S_UNINSTALL_SUB
 {L"Remove Halloween effects", L"Убрать Хеллоуин", L"Прибрати Гелловін", L"Elimină efectele Halloween", L"Halloween-Effekte entfernen"},
 // S_REMOVED_TITLE
 {L"UltimateDesktopEffects removed", L"UltimateDesktopEffects удалён", L"UltimateDesktopEffects видалено", L"UltimateDesktopEffects a fost eliminat", L"UltimateDesktopEffects wurde entfernt"},
 // S_REMOVED_TEXT
 {L"The treats and spiders have gone.", L"Сладости и пауки убраны.", L"Солодощі та павуки прибрані.", L"Dulciurile și păianjenii au dispărut.", L"Süßigkeiten und Spinnen wurden entfernt."},
 // S_UNINSTALL_LINK
 {L"Uninstall UltimateDesktopEffects", L"Удалить UltimateDesktopEffects", L"Видалити UltimateDesktopEffects", L"Dezinstalează UltimateDesktopEffects", L"UltimateDesktopEffects deinstallieren"},
 // S_MODE_CANDY
 {L"Candy", L"Сладости", L"Солодощі", L"Dulciuri", L"Süßigkeiten"},
 // S_MODE_SPIDERS
 {L"Spiders", L"Пауки", L"Павуки", L"Păianjeni", L"Spinnen"},
 // S_MODE_BOTH
 {L"Both", L"Всё", L"Разом", L"Ambele", L"Beides"},
 // S_CARD_SPIDERS
 {L"Spiders and threads", L"Пауки и нити", L"Павуки та нитки", L"Păianjeni și fire", L"Spinnen und Fäden"},
 // S_SPIDER_INTERVAL
 {L"Spider interval", L"Интервал пауков", L"Інтервал павуків", L"Interval păianjeni", L"Spinnenabstand"},
 // S_SPIDER_SPEED
 {L"Climbing speed", L"Скорость спуска", L"Швидкість спуску", L"Viteză de coborâre", L"Abseiltempo"},
 // S_PARTIAL_DESCENTS
 {L"Partial descents", L"Неполный спуск", L"Частковий спуск", L"Coborâri parțiale", L"Teilweises Abseilen"},
 // S_HANG_TIME
 {L"Pause before deciding", L"Пауза перед решением", L"Пауза перед рішенням", L"Pauză înainte de alegere", L"Hängepause"},
 // S_WEB_OPACITY
 {L"Thread opacity", L"Видимость нитей", L"Видимість ниток", L"Opacitate fire", L"Deckkraft: Fäden"},
 // S_CUT_RADIUS
 {L"Click tolerance", L"Точность щелчка", L"Допуск кліку", L"Toleranță clic", L"Klicktoleranz"},
 // S_CUT_WEBS
 {L"Cut threads with a click", L"Перерезать нити щелчком", L"Перерізати нитки кліком", L"Taie firele cu un clic", L"Fäden per Klick schneiden"},
 // S_TEST_SPIDER
 {L"Release a spider", L"Добавить паука", L"Додати павука", L"Adaugă un păianjen", L"Spinne abseilen"},
 // S_CANDY_VARIANTS
 {L"6 candy variants", L"6 вариантов сладостей", L"6 варіантів солодощів", L"6 variante de dulciuri", L"6 Süßigkeitenvarianten"},
 // S_SPIDER_VARIANTS
 {L"4 spider types", L"4 вида пауков", L"4 типи павуків", L"4 tipuri de păianjeni", L"4 Spinnenarten"},
};
static_assert(sizeof(kText)/sizeof(kText[0])==S_COUNT,"translation count mismatch");
int CurrentLang(){return ::CurrentLang();}
const wchar_t* Tr(Str id){return kText[id][CurrentLang()];}
}
