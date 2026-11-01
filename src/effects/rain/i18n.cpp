#include "i18n.h"
#include "../../settings_api.h"
namespace rainfx {
const wchar_t* const kLangCodes[LANG_COUNT]={L"EN",L"RU",L"UA",L"RO",L"DE"};
static const wchar_t* const kText[][LANG_COUNT]={
 // S_TRAY_TIP
 {L"UltimateDesktopEffects: rain and lightning", L"UltimateDesktopEffects: дождь и молнии", L"UltimateDesktopEffects: дощ і блискавки", L"UltimateDesktopEffects: ploaie și fulgere", L"UltimateDesktopEffects: Regen und Gewitter"},
 // S_WELCOME_TITLE
 {L"UltimateDesktopEffects is running", L"UltimateDesktopEffects запущен", L"UltimateDesktopEffects запущено", L"UltimateDesktopEffects rulează", L"UltimateDesktopEffects läuft"},
 // S_WELCOME_TEXT
 {L"The raindrop is in the tray. Left click opens settings, right click opens the quick menu.", L"Капля в трее. Левый клик — настройки, правый — быстрое меню.", L"Крапля в треї. Лівий клік — налаштування, правий — швидке меню.", L"Picătura este în zona de notificare. Clic stânga: setări. Clic dreapta: meniu.", L"Das Tropfensymbol ist im Infobereich. Linksklick: Einstellungen. Rechtsklick: Schnellmenü."},
 // S_SETTINGS_TITLE
 {L"UltimateDesktopEffects: settings", L"UltimateDesktopEffects: настройки", L"UltimateDesktopEffects: налаштування", L"UltimateDesktopEffects: setări", L"UltimateDesktopEffects: Einstellungen"},
 // S_SUBTITLE
 {L"Rain, ripples and thunderstorms on your desktop", L"Дождь, рябь и гроза на рабочем столе", L"Дощ, брижі та гроза на робочому столі", L"Ploaie, valuri și furtuni pe desktop", L"Regen, Wasser und Gewitter auf deinem Desktop"},
 // S_CARD_SNOWFALL
 {L"Rainfall", L"Дождь", L"Дощ", L"Ploaie", L"Regen"},
 // S_MODE_TOP
 {L"Over windows", L"Поверх окон", L"Поверх вікон", L"Peste ferestre", L"Vor Fenstern"},
 // S_MODE_BEHIND
 {L"Behind windows", L"За окнами", L"За вікнами", L"Sub ferestre", L"Hinter Fenstern"},
 // S_PRESET_LIGHT
 {L"Drizzle", L"Морось", L"Мряка", L"Burniță", L"Niesel"},
 // S_PRESET_NORMAL
 {L"Rain", L"Дождь", L"Дощ", L"Ploaie", L"Regen"},
 // S_PRESET_STRONG
 {L"Downpour", L"Ливень", L"Злива", L"Aversă", L"Starkregen"},
 // S_PRESET_BLIZZARD
 {L"Storm", L"Гроза", L"Гроза", L"Furtună", L"Gewitter"},
 // S_INTENSITY
 {L"Intensity", L"Интенсивность", L"Інтенсивність", L"Intensitate", L"Regenstärke"},
 // S_FALL_SPEED
 {L"Fall speed", L"Скорость падения", L"Швидкість падіння", L"Viteza căderii", L"Fallgeschwindigkeit"},
 // S_WIND_ANGLE
 {L"Wind angle", L"Угол ветра", L"Кут вітру", L"Unghiul vântului", L"Windwinkel"},
 // S_GUSTS
 {L"Wind gusts", L"Порывы ветра", L"Пориви вітру", L"Rafale de vânt", L"Windböen"},
 // S_FLAKE_SIZE
 {L"Drop size", L"Размер капель", L"Розмір крапель", L"Mărimea picăturii", L"Tropfengröße"},
 // S_CARD_PILES
 {L"Water on edges", L"Вода на краях", L"Вода на краях", L"Apă pe margini", L"Wasser auf Kanten"},
 // S_ACCUMULATION
 {L"Accumulation", L"Накопление воды", L"Накопичення води", L"Acumulare", L"Ansammlung"},
 // S_PILE_TASKBAR
 {L"Taskbar water depth", L"Вода над панелью", L"Вода над панеллю", L"Apă pe bara de activități", L"Wasser: Taskleiste"},
 // S_PILE_WINDOWS
 {L"Window water depth", L"Вода на окнах", L"Вода на вікнах", L"Apă pe ferestre", L"Wasser: Fenster"},
 // S_DRIFTS
 {L"Waves and ripples", L"Волны и рябь", L"Хвилі та брижі", L"Valuri și unde", L"Wellen und Kräuselung"},
 // S_MELT
 {L"Evaporation", L"Испарение", L"Випаровування", L"Evaporare", L"Verdunstung"},
 // S_CURSOR_RADIUS
 {L"Cursor radius", L"Радиус курсора", L"Радіус курсора", L"Raza cursorului", L"Mausradius"},
 // S_SMOOTHNESS
 {L"Smoothness", L"Плавность", L"Плавність", L"Fluiditate", L"Bildrate"},
 // S_CARD_BEHAVIOR
 {L"Behavior", L"Поведение", L"Поведінка", L"Comportament", L"Verhalten"},
 // S_SNOW_ON
 {L"Weather enabled", L"Погода включена", L"Погоду ввімкнено", L"Efecte meteo pornite", L"Wetter einschalten"},
 // S_ON_WINDOWS
 {L"Collect water on windows", L"Вода на окнах", L"Вода на вікнах", L"Acumulează apă pe ferestre", L"Wasser auf Fenstern"},
 // S_ON_TASKBAR
 {L"Collect water on taskbar", L"Вода над панелью задач", L"Вода над панеллю завдань", L"Apă pe bara de activități", L"Wasser auf der Taskleiste"},
 // S_CURSOR_KNOCK
 {L"Splash water with cursor", L"Разбрызгивать курсором", L"Розбризкувати курсором", L"Stropește cu cursorul", L"Wasser mit Maus verdrängen"},
 // S_CRYSTALS
 {L"Large drop share", L"Крупные капли", L"Великі краплі", L"Picături mari", L"Große Tropfen"},
 // S_PAUSE_GAMES
 {L"Hide in games and fullscreen apps", L"Скрывать в играх и полноэкранных приложениях", L"Ховати в іграх і повноекранних програмах", L"Ascunde în jocuri și aplicații pe tot ecranul", L"In Spielen / Vollbild pausieren"},
 // S_AUTOSTART
 {L"Start with Windows", L"Запускать вместе с Windows", L"Запускати разом з Windows", L"Pornește odată cu Windows", L"Mit Windows starten"},
 // S_LANGUAGE
 {L"Language", L"Язык", L"Мова", L"Limbă", L"Sprache"},
 // S_SHAKE
 {L"Shake off water", L"Стряхнуть воду", L"Струсити воду", L"Scutură apa", L"Wasser abschütteln"},
 // S_DEFAULTS
 {L"Defaults", L"По умолчанию", L"За замовчуванням", L"Implicit", L"Standardwerte"},
 // S_DONE
 {L"Done", L"Готово", L"Готово", L"Gata", L"Fertig"},
 // S_MODE
 {L"Mode", L"Режим", L"Режим", L"Mod", L"Anzeigemodus"},
 // S_SNOWFALL_STRENGTH
 {L"Weather preset", L"Режим погоды", L"Режим погоди", L"Presetare meteo", L"Wetterprofil"},
 // S_SMOOTHNESS_FPS
 {L"Smoothness, FPS", L"Плавность, FPS", L"Плавність, FPS", L"Fluiditate, FPS", L"Bildrate (FPS)"},
 // S_SHAKE_ALL
 {L"Shake off all water", L"Стряхнуть всю воду", L"Струсити всю воду", L"Scutură toată apa", L"Alles Wasser abschütteln"},
 // S_ALL_SETTINGS
 {L"All settings…", L"Все настройки…", L"Усі налаштування…", L"Toate setările…", L"Alle Einstellungen…"},
 // S_EXIT
 {L"Exit", L"Выход", L"Вихід", L"Ieșire", L"Beenden"},
 // S_INSTALL_TITLE
 {L"Install UltimateDesktopEffects", L"Установка UltimateDesktopEffects", L"Встановлення UltimateDesktopEffects", L"Instalare UltimateDesktopEffects", L"UltimateDesktopEffects installieren"},
 // S_UPDATE_TITLE
 {L"Update UltimateDesktopEffects", L"Обновление UltimateDesktopEffects", L"Оновлення UltimateDesktopEffects", L"Actualizare UltimateDesktopEffects", L"UltimateDesktopEffects aktualisieren"},
 // S_INSTALL_SUB
 {L"Rain and thunderstorms on your desktop", L"Дождь и гроза на рабочем столе", L"Дощ і гроза на робочому столі", L"Ploaie și furtuni pe desktop", L"Regen und Gewitter auf deinem Desktop"},
 // S_WHAT_INSTALLED
 {L"What gets installed", L"Что будет установлено", L"Що буде встановлено", L"Ce se instalează", L"Was installiert wird"},
 // S_INSTALL_DESC
 {L"Rain falls over or behind windows. Water collects on window edges and the taskbar, with splashes and optional lightning. Based on LayerSnow by flipkips (MIT).", L"Дождь поверх окон или за ними. Вода на краях окон и панели задач, брызги и отключаемые молнии. На основе UltimateDesktopEffects от flipkips (MIT).", L"Дощ поверх вікон або за ними. Вода на краях вікон і панелі завдань, бризки та вимикні блискавки. На основі UltimateDesktopEffects від flipkips (MIT).", L"Ploaie peste ferestre sau în spatele lor. Apă pe margini, stropi și fulgere opționale. Bazat pe UltimateDesktopEffects de flipkips (MIT).", L"Regen vor oder hinter Fenstern. Wasser sammelt sich auf Fensterkanten und der Taskleiste, mit Spritzern und optionalen Blitzen. Basis: LayerSnow von flipkips (MIT)."},
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
 // S_INSTALL_DONE
 {L"Done! Start UltimateDesktopEffects from the Start menu.", L"Готово! Запустите UltimateDesktopEffects из меню «Пуск».", L"Готово! Запустіть UltimateDesktopEffects з меню «Пуск».", L"Gata! Pornește UltimateDesktopEffects din meniul Start.", L"Fertig! UltimateDesktopEffects lässt sich im Startmenü öffnen."},
 // S_UNINSTALL_TITLE
 {L"Uninstall UltimateDesktopEffects", L"Удаление UltimateDesktopEffects", L"Видалення UltimateDesktopEffects", L"Dezinstalare UltimateDesktopEffects", L"UltimateDesktopEffects deinstallieren"},
 // S_UNINSTALL_SUB
 {L"Remove the weather overlay", L"Убрать погодные эффекты", L"Прибрати погодні ефекти", L"Elimină efectele meteo", L"Wettereffekte entfernen"},
 // S_UNINSTALL_CARD
 {L"What will be removed", L"Что будет удалено", L"Що буде видалено", L"Ce se elimină", L"Was entfernt wird"},
 // S_UNINSTALL_DESC
 {L"The program, its Start menu and desktop shortcuts, and autostart.", L"Программа, её ярлыки в меню «Пуск» и на рабочем столе, автозапуск.", L"Програма, її ярлики в меню «Пуск» і на робочому столі, автозапуск.", L"Programul, comenzile rapide din meniul Start și de pe desktop, pornirea automată.", L"Die Anwendung, Verknüpfungen und der Autostart-Eintrag."},
 // S_DELETE_SETTINGS
 {L"Also delete my settings", L"Удалить и мои настройки", L"Видалити й мої налаштування", L"Șterge și setările mele", L"Auch meine Einstellungen löschen"},
 // S_UNINSTALL
 {L"Uninstall", L"Удалить", L"Видалити", L"Dezinstalează", L"Deinstallieren"},
 // S_REMOVED_TITLE
 {L"UltimateDesktopEffects removed", L"UltimateDesktopEffects удалён", L"UltimateDesktopEffects видалено", L"UltimateDesktopEffects a fost eliminat", L"UltimateDesktopEffects wurde entfernt"},
 // S_REMOVED_TEXT
 {L"The rain has stopped.", L"Дождь закончился.", L"Дощ закінчився.", L"Ploaia s-a oprit.", L"Der Regen hat aufgehört."},
 // S_CLOSE
 {L"Close", L"Закрыть", L"Закрити", L"Închide", L"Schließen"},
 // S_UNINSTALL_LINK
 {L"Uninstall UltimateDesktopEffects", L"Удалить UltimateDesktopEffects", L"Видалити UltimateDesktopEffects", L"Dezinstalează UltimateDesktopEffects", L"UltimateDesktopEffects deinstallieren"},
 // S_CURSOR_WIND
 {L"Wind follows cursor", L"Ветер за курсором", L"Вітер за курсором", L"Vântul după cursor", L"Wind folgt der Maus"},
 // S_SHOW_FALLING
 {L"Show falling rain", L"Показывать дождь", L"Показувати дощ", L"Arată ploaia", L"Fallenden Regen anzeigen"},
 // S_AUTHOR
 {L"Based on LayerSnow by flipkips", L"Основа: LayerSnow от flipkips", L"Основа: LayerSnow від flipkips", L"Bază: LayerSnow de flipkips", L"Basis: LayerSnow von flipkips"},
 // S_STAR_GITHUB
 {L"Original project on GitHub", L"Оригинальный проект на GitHub", L"Оригінальний проєкт на GitHub", L"Proiectul original pe GitHub", L"Originalprojekt auf GitHub"},
 // S_WEATHER_SETTINGS
 {L"Weather settings…", L"Настройки погоды…", L"Налаштування погоди…", L"Setări meteo…", L"Wettereinstellungen…"},
 // S_CARD_LIGHTNING
 {L"Thunderstorm", L"Гроза", L"Гроза", L"Furtună", L"Gewitter"},
 // S_LIGHTNING
 {L"Lightning enabled", L"Молнии включены", L"Блискавки ввімкнено", L"Fulgere pornite", L"Blitze einschalten"},
 // S_SCREEN_FLASH
 {L"Illuminate the desktop", L"Подсвечивать рабочий стол", L"Підсвічувати робочий стіл", L"Iluminează desktopul", L"Desktop aufhellen"},
 // S_LIGHTNING_INTERVAL
 {L"Average strike interval", L"Интервал молний", L"Інтервал блискавок", L"Interval între fulgere", L"Mittlerer Blitzabstand"},
 // S_LIGHTNING_BRIGHTNESS
 {L"Lightning brightness", L"Яркость молний", L"Яскравість блискавок", L"Luminozitate fulgere", L"Blitzhelligkeit"},
 // S_LIGHTNING_BRANCHES
 {L"Branching", L"Ветвление", L"Розгалуження", L"Ramificare", L"Verzweigung"},
 // S_RAIN_OPACITY
 {L"Rain opacity", L"Непрозрачность дождя", L"Непрозорість дощу", L"Opacitatea ploii", L"Deckkraft: Regen"},
 // S_WATER_OPACITY
 {L"Water opacity", L"Непрозрачность воды", L"Непрозорість води", L"Opacitatea apei", L"Deckkraft: Wasser"},
 // S_STREAK_LENGTH
 {L"Rain streak length", L"Длина следа капель", L"Довжина сліду крапель", L"Lungime dâre", L"Länge der Regenstreifen"},
 // S_SPLASHES
 {L"Splash amount", L"Количество брызг", L"Кількість бризок", L"Cantitate stropi", L"Spritzereffekte"},
 // S_TEST_LIGHTNING
 {L"Test lightning", L"Проверить молнию", L"Перевірити блискавку", L"Testează fulgerul", L"Blitz testen"},
 // S_LIGHTNING_NOTE
 {L"Enable lightning to test it. The Storm preset turns lightning on. Desktop illumination is optional.", L"Включите молнии для проверки. Режим «Гроза» включает молнии. Подсветка стола — отдельно.", L"Увімкніть блискавки для перевірки. Режим «Гроза» вмикає блискавки. Підсвічування — окремо.", L"Activează fulgerele pentru test. Presetarea Furtună le activează. Iluminarea este opțională.", L"Zum Testen Blitze einschalten. Das Profil „Gewitter“ aktiviert Blitze. Die Desktop-Aufhellung ist separat schaltbar."},
 // S_CARD_LOOK
 {L"Appearance", L"Внешний вид", L"Зовнішній вигляд", L"Aspect", L"Darstellung"},
 // S_LOOK_NOTE
 {L"All changes are live and saved automatically. Rain, water and lightning share the over / behind windows mode.", L"Изменения применяются и сохраняются автоматически. Режим перед/за окнами общий для всех эффектов.", L"Зміни застосовуються та зберігаються автоматично. Режим перед/за вікнами спільний для всіх ефектів.", L"Modificările se aplică și se salvează automat. Modul peste/în spate se aplică tuturor efectelor.", L"Änderungen gelten sofort und werden automatisch gespeichert. Der Modus vor/hinter Fenstern gilt auch für Blitze."},
 // S_LICENSE
 {L"Credits / MIT license", L"Авторы / лицензия MIT", L"Автори / ліцензія MIT", L"Autori / licență MIT", L"Mitwirkende / MIT-Lizenz"},
};
static_assert(sizeof(kText)/sizeof(kText[0])==S_COUNT,"translation count mismatch");
int CurrentLang(){return ::CurrentLang();}
const wchar_t* Tr(Str id){return kText[id][CurrentLang()];}
}
