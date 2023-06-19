# TransportCatalogue

Транспортный справочник. Функционал: \
Получение и вывод данных в формате JSON. \
Сериализация и десериализация транспортного справочника с использованием Google Protobuf. \
Визуализация карты маршрутов в формате SVG. \
Поиск кратчайшего пути по заданным условиям. 

## Сборка

> 1. Скачайте и соберите Google Protobuf. \
> 2. Создайте папку для сборки программы. \
> 3. Откройте консоль в данной папке и введите в консоли : `cmake <путь к файлу CMakeLists.txt> -DCMAKE_PREFIX_PATH= <путь к собранной библиотеке Protobuf>`. \
> Или в файле CMakeLists.txt раскомментировать строчку `set (CMAKE_PREFIX_PATH "<путь к собранной библиотеке Protobuf>")`
> 4. Введите команду : `cmake --build .` . \
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

```
</details>

## Планы по доработке

Обеспечить зарузку запросов `make_base` и `process_requests` из файлов .json и сохранение результатов в файл .json.
