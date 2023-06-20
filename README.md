# TransportCatalogue

Транспортный справочник. Функционал: \
Получение и вывод данных в формате JSON. \
Сериализация и десериализация транспортного справочника с использованием Google Protobuf. \
Визуализация карты маршрутов в формате SVG. \
Поиск кратчайшего пути по заданным условиям. 

## Работа с транспортным каталогом

Для работы с транспортным каталогом, Json и SVG разработан фасад. Основные методы фасада:

```с++
class TransportCatalogueHandler {
public:

    // Конструктор фасада
    TransportCatalogueHandler(const transport_catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer& renderer
        , const transport_router::TransportRouter& router)
        : catalogue_(catalogue)
        , renderer_(renderer)
        , router_(router)
    {
    }

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusInfo> GetBusInfo(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::set<std::string_view>* GetBusesByStop(const std::string_view& stop_name) const;

    // Возвращает все маршруты
    const std::unordered_map<std::string_view, const Bus*>& GetBuses() const;

    // Возвращает все остановки
    const std::unordered_map<std::string_view, const Stop*>& GetStops() const;

    // Построение короткого маршрута
    std::optional <graph::Router<transport_router::RouteWeight>::RouteInfo> BuildRouter (const std::string_view stop_name_from, const std::string_view stop_name_to) const;

    // Получение графа
    const graph::DirectedWeightedGraph<transport_router::RouteWeight>& GetGraph() const;

    // Получение заданный настроек маршрута
    const transport_router::RouterSettings& GetRouterSettings() const;

    // Получение названия маршрута по id
    const std::string_view GetStopNameFromID(size_t id) const;
    
    // Построение карты
    svg::Document RenderMap() const;

private:
    const transport_catalogue::TransportCatalogue& catalogue_;
    const renderer::MapRenderer& renderer_;
    const transport_router::TransportRouter& router_;
};
```

## Сборка

> 1. Скачайте и соберите Google Protobuf. 
> 2. Создайте папку для сборки программы.
> 3. Откройте консоль в данной папке и введите в консоли : `cmake <путь к файлу CMakeLists.txt> -DCMAKE_PREFIX_PATH=<путь к собранной библиотеке Protobuf>`. \
> Или в файле CMakeLists.txt раскомментировать строчку `set (CMAKE_PREFIX_PATH "<путь к собранной библиотеке Protobuf>")`. Открыть консоль в данной папке и введите : `cmake <путь к файлу CMakeLists.txt>`.
> 4. Введите команду : `cmake --build .` .
> 5. После сборки в папке сборки появится исполняемый файл `transport_catalogue.exe`.

## Требования к сборке

Компилятор С++ с поддержкой стандарта C++17 или новее. \
Google Protobuf. 

## Использование собранной версии программы

В `make_base` и `process_requests` в наименованиях остановок и маршрутов поддерживаются только латинские буквы (верхнего или нижнего регистра) и цифры.

### Формирование транспортного каталога

Запустите собранную программу с ключом : `./transport_catalogue make_base`.\
В запущенной программе введите `make_base` - запрос, который сформирует на его основе транспортный каталог. \
В папке с программой появится файл типа `.db` (название файла зависит от `"serialization_settings"` запроса `make_base`). В данном файле будет сохранен каталог в двоичном виде.\
В дальнейшем этот сохраненный каталог можно будет "разворачивать" для формирования ответов на запросы.

`make_base` - запрос должен представлять собой словарь JSON со следующими разделами (ключами) :\
`serialization_settings` - настройки сериализации.\
`routing_settings` - настройки маршрутизации. \
`render_settings` - настройки отрисовки. \
`base_requests` - массив данных об остановках и маршрутах.

#### Разделы запроса

<details>
  <summary>"serialization_settings"</summary>

```json
    "serialization_settings": {
          "file": "transport_catalogue.db" - наименование файла для сериализации
    }
```
</details>

<details>
  <summary>"routing_settings"</summary>

```json
    "routing_settings": {
        "bus_wait_time": 2, - время ожидания автобуса
        "bus_velocity": 30 - скорость автобуса
    }
```
</details>

<details>
  <summary>"render_settings"</summary>

```json
    "render_settings": {
        "width": 1200 - ширина изображения в пикселях
        "height": 500, - высота изображения в пикселях
        "padding": 50, - отступ краёв карты от границ SVG-документа
        "stop_radius": 5, - радиус окружностей, которыми обозначаются остановки
        "line_width": 14, - толщина линий, которыми рисуются автобусные маршруты
        "bus_label_font_size": 20, - размер текста, которым написаны названия автобусных маршрутов
        "bus_label_offset": [ - смещение надписи с названием маршрута относительно координат конечной остановки на карте
            7,
            15
        ],
        "stop_label_font_size": 18, - размер текста, которым отображаются названия остановок
        "stop_label_offset": [ - смещение названия остановки относительно её координат на карте
            7,
            -3
        ],
        "underlayer_color": [ - цвет подложки под названиями остановок и маршрутов
            255,
            255,
            255,
            0.85
        ],
        "underlayer_width": 3, - толщина подложки под названиями остановок и маршрутов
        "color_palette": [ - цветовая палитра. Непустой массив
            "green",
            [
                255,
                160,
                0
            ],
            "red"
        ]
    }

  Цвет может задаваться в трех форматах: в виде строки, rgb или rgba
```
</details>

Массив данных "base_requests" поддерживает ввод следующих типов:

<details>
  <summary>Тип "Stop" - вводит в справочник информацию об остановке </summary>

```json 
    {
        "type": "Stop", - наименование типа
        "name": "Lisa Chaikina street", - наименование остановки
        "latitude": 43.590317, - координаты
        "longitude": 39.746833, - координаты
        "road_distances": { - задание пользовательского расстояния до остановок (может быть пустым)
            "Elektroseti": 4300, - название остановки : расстояние
            "Dokuchaeva street": 2000 - название остановки : расстояние
        }
    }
```
</details>

<details>
  <summary>Тип "Bus" - вводит в справочник информацию о маршруте </summary>

```json 
  {
      "type": "Bus", - наименование типа
      "name": "14", - наименование маршрута
      "stops": [ - остановки маршрута
          "Lisa Chaikina street",
          "Elektroseti",
          "Riverskii most",
          "Hotel Sochi",
          "Kubanskaya street",
          "On demand",
          "Dokuchaeva street",
          "Lisa Chaikina street"
      ],
      "is_roundtrip": true - является ли маршрут круговым (true/false)
  }
```
</details>

<details>
  <summary>Пример корректного запроса "make_base" </summary>

```json 
    {
      "serialization_settings": {
          "file": "transport_catalogue.db"
      },
      "routing_settings": {
          "bus_wait_time": 2,
          "bus_velocity": 30
      },
      "render_settings": {
          "width": 1200,
          "height": 500,
          "padding": 50,
          "stop_radius": 5,
          "line_width": 14,
          "bus_label_font_size": 20,
          "bus_label_offset": [
              7,
              15
          ],
          "stop_label_font_size": 18,
          "stop_label_offset": [
              7,
              -3
          ],
          "underlayer_color": [
              255,
              255,
              255,
              0.85
          ],
          "underlayer_width": 3,
          "color_palette": [
              "green",
              [
                  255,
                  160,
                  0
              ],
              "red"
          ]
      },
      "base_requests": [
          {
              "type": "Bus",
              "name": "14",
              "stops": [
                  "Lisa Chaikina street",
                  "Elektroseti",
                  "Riverskii most",
                  "Hotel Sochi",
                  "Kubanskaya street",
                  "On demand",
                  "Dokuchaeva street",
                  "Lisa Chaikina street"
              ],
              "is_roundtrip": true
          },
          {
              "type": "Bus",
              "name": "24",
              "stops": [
                  "Dokuchaeva street",
                  "Parallel street",
                  "Elektroseti",
                  "Sanatorium Rodina"
              ],
              "is_roundtrip": false
          },
          {
              "type": "Bus",
              "name": "114",
              "stops": [
                  "Marine Station",
                  "Riverskii most"
              ],
              "is_roundtrip": false
          },
          {
              "type": "Stop",
              "name": "Lisa Chaikina street",
              "latitude": 43.590317,
              "longitude": 39.746833,
              "road_distances": {
                  "Elektroseti": 4300,
                  "Dokuchaeva street": 2000
              }
          },
          {
              "type": "Stop",
              "name": "Marine Station",
              "latitude": 43.581969,
              "longitude": 39.719848,
              "road_distances": {
                  "Riverskii most": 850
              }
          },
          {
              "type": "Stop",
              "name": "Elektroseti",
              "latitude": 43.598701,
              "longitude": 39.730623,
              "road_distances": {
                  "Sanatorium Rodina": 4500,
                  "Parallel street": 1200,
                  "Riverskii most": 1900
              }
          },
          {
              "type": "Stop",
              "name": "Riverskii most",
              "latitude": 43.587795,
              "longitude": 39.716901,
              "road_distances": {
                  "Marine Station": 850,
                  "Hotel Sochi": 1740
              }
          },
          {
              "type": "Stop",
              "name": "Hotel Sochi",
              "latitude": 43.578079,
              "longitude": 39.728068,
              "road_distances": {
                  "Kubanskaya street": 320
              }
          },
          {
              "type": "Stop",
              "name": "Kubanskaya street",
              "latitude": 43.578509,
              "longitude": 39.730959,
              "road_distances": {
                  "On demand": 370
              }
          },
          {
              "type": "Stop",
              "name": "On demand",
              "latitude": 43.579285,
              "longitude": 39.733742,
              "road_distances": {
                  "Dokuchaeva street": 600
              }
          },
          {
              "type": "Stop",
              "name": "Dokuchaeva street",
              "latitude": 43.585586,
              "longitude": 39.733879,
              "road_distances": {
                  "Parallel street": 1100
              }
          },
          {
              "type": "Stop",
              "name": "Parallel street",
              "latitude": 43.590041,
              "longitude": 39.732886,
              "road_distances": {}
          },
          {
              "type": "Stop",
              "name": "Sanatorium Rodina",
              "latitude": 43.601202,
              "longitude": 39.715498,
              "road_distances": {}
          }
      ]
  }
```
</details>

### Использование сформированного транспортного каталога

Запустите собранную программу с ключом : `./transport_catalogue process_requests`.\
В запущенной программе введите `process_requests` - запрос. В данном файле в настройках `"serialization_settings"` должно быть указано имя существующего файла с двоичным представлением сформированного транспортного каталога. \
После десериализации каталога, программа последовательно обойдет запросы из `"stat_requests"` и выведет сформированные ответы в консоль.

`process_requests` - запрос должен представлять собой словарь JSON со следующими разделами (ключами) :\
`serialization_settings` - настройки сериализации.\
`stat_requests` - массив запросов к каталогу.

Массив запросов к каталогу поддерживает ввод следующих типов:

<details>
  <summary>Тип "Bus" - запрос на вывод информации о маршруте </summary>

```json 
  {
      "id": 218563507, - id запроса
      "type": "Bus", - тип запроса
      "name": "14" - имя автобуса из справочника
  }
```
</details>

<details>
  <summary>Тип "Stop" - запрос на вывод информации об остановке </summary>

```json 
  {
      "id": 508658276, - id запроса
      "type": "Stop", - тип запроса
      "name": "Elektroseti" - имя остановки из справочника
  }
```
</details>

<details>
  <summary>Тип "Route" - запрос на вывод кратчайшего маршрута по заданным параметрам "routing_settings" </summary>

```json 
  {
      "id": 1964680131, - id запроса
      "type": "Route", - тип запроса
      "from": "Marine Station", - наименование остановки начала маршрута
      "to": "Parallel street" - наименование остановки конца маршрута
  }
```
</details>

<details>
  <summary>Тип "Map" - запрос на построение карты каталога в формате SVG </summary>

```json 
  {
      "id": 1359372752, - id запроса
      "type": "Map" - тип запроса
  }
```
</details>

<details>
  <summary>Пример корректного запроса "process_requests" </summary>

```json 
  {
      "serialization_settings": {
          "file": "transport_catalogue.db"
      },
      "stat_requests": [
          {
              "id": 218563507,
              "type": "Bus",
              "name": "14"
          },
          {
              "id": 508658276,
              "type": "Stop",
              "name": "Elektroseti"
          },
          {
              "id": 1964680131,
              "type": "Route",
              "from": "Marine Station",
              "to": "Parallel street"
          },
          {
              "id": 1359372752,
              "type": "Map"
          }
      ]
  }
```
</details>

<details>
  <summary>Пример вывода:</summary>

```json
[
  {
    "curvature": 1.60481,
    "request_id": 218563507,
    "route_length": 11230,
    "stop_count": 8,
    "unique_stop_count": 7
  },
  {
    "buses": [
      "14",
      "24"
    ],
    "request_id": 508658276
  },
  {
    "items": [
      {
        "stop_name": "Marine Station",
        "time": 2,
        "type": "Wait"
      },
      {
        "bus": "114",
        "span_count": 1,
        "time": 472.222,
        "type": "Bus"
      },
      {
        "stop_name": "Riverskii most",
        "time": 2,
        "type": "Wait"
      },
      {
        "bus": "14",
        "span_count": 4,
        "time": 1683.33,
        "type": "Bus"
      },
      {
        "stop_name": "Dokuchaeva street",
        "time": 2,
        "type": "Wait"
      },
      {
        "bus": "24",
        "span_count": 1,
        "time": 611.111,
        "type": "Bus"
      }
    ],
    "request_id": 1964680131,
    "total_time": 2772.67
  },
  {
    "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n  <polyline points=\"125.25,382.708 74.2702,281.925 125.25,382.708\" fill=\"none\" stroke=\"green\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\" />\n  <polyline points=\"592.058,238.297 311.644,93.2643 74.2702,281.925 267.446,450 317.457,442.562 365.599,429.138 367.969,320.138 592.058,238.297\" fill=\"none\" stroke=\"rgb(255,160,0)\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\" />\n  <polyline points=\"367.969,320.138 350.791,243.072 311.644,93.2643 50,50 311.644,93.2643 350.791,243.072 367.969,320.138\" fill=\"none\" stroke=\"red\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\" />\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"125.25\" y=\"382.708\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  <text fill=\"green\" x=\"125.25\" y=\"382.708\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"74.2702\" y=\"281.925\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  <text fill=\"green\" x=\"74.2702\" y=\"281.925\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">114</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"592.058\" y=\"238.297\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">14</text>\n  <text fill=\"rgb(255,160,0)\" x=\"592.058\" y=\"238.297\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">14</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"367.969\" y=\"320.138\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">24</text>\n  <text fill=\"red\" x=\"367.969\" y=\"320.138\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">24</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"50\" y=\"50\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">24</text>\n  <text fill=\"red\" x=\"50\" y=\"50\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\">24</text>\n  <circle cx=\"367.969\" cy=\"320.138\" r=\"5\" fill=\"white\" />\n  <circle cx=\"311.644\" cy=\"93.2643\" r=\"5\" fill=\"white\" />\n  <circle cx=\"267.446\" cy=\"450\" r=\"5\" fill=\"white\" />\n  <circle cx=\"317.457\" cy=\"442.562\" r=\"5\" fill=\"white\" />\n  <circle cx=\"592.058\" cy=\"238.297\" r=\"5\" fill=\"white\" />\n  <circle cx=\"125.25\" cy=\"382.708\" r=\"5\" fill=\"white\" />\n  <circle cx=\"365.599\" cy=\"429.138\" r=\"5\" fill=\"white\" />\n  <circle cx=\"350.791\" cy=\"243.072\" r=\"5\" fill=\"white\" />\n  <circle cx=\"74.2702\" cy=\"281.925\" r=\"5\" fill=\"white\" />\n  <circle cx=\"50\" cy=\"50\" r=\"5\" fill=\"white\" />\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"367.969\" y=\"320.138\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Dokuchaeva street</text>\n  <text fill=\"black\" x=\"367.969\" y=\"320.138\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Dokuchaeva street</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"311.644\" y=\"93.2643\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Elektroseti</text>\n  <text fill=\"black\" x=\"311.644\" y=\"93.2643\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Elektroseti</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"267.446\" y=\"450\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Hotel Sochi</text>\n  <text fill=\"black\" x=\"267.446\" y=\"450\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Hotel Sochi</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"317.457\" y=\"442.562\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Kubanskaya street</text>\n  <text fill=\"black\" x=\"317.457\" y=\"442.562\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Kubanskaya street</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"592.058\" y=\"238.297\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Lisa Chaikina street</text>\n  <text fill=\"black\" x=\"592.058\" y=\"238.297\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Lisa Chaikina street</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"125.25\" y=\"382.708\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Marine Station</text>\n  <text fill=\"black\" x=\"125.25\" y=\"382.708\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Marine Station</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"365.599\" y=\"429.138\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">On demand</text>\n  <text fill=\"black\" x=\"365.599\" y=\"429.138\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">On demand</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"350.791\" y=\"243.072\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Parallel street</text>\n  <text fill=\"black\" x=\"350.791\" y=\"243.072\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Parallel street</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"74.2702\" y=\"281.925\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Riverskii most</text>\n  <text fill=\"black\" x=\"74.2702\" y=\"281.925\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Riverskii most</text>\n  <text fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"50\" y=\"50\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Sanatorium Rodina</text>\n  <text fill=\"black\" x=\"50\" y=\"50\" dx=\"7\" dy=\"-3\" font-size=\"18\" font-family=\"Verdana\">Sanatorium Rodina</text>\n</svg>",
    "request_id": 1359372752
  }
]
```
</details>

## Планы по доработке

Обеспечить зарузку запросов `make_base` и `process_requests` из файлов .json и сохранение результатов в файл .json.
