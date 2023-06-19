# TransportCatalogue

Транспортный справочник. Функционал: \
Получение и вывод данных в формате JSON. \
Сериализация и десериализация транспортного справочника с использованием Google Protobuf. \
Визуализация карты маршрутов в формате SVG. \
Поиск кратчайшего пути по заданным условиям. 

## Сборка

> Скачайте и соберите Google Protobuf. \
> Создайте папку для сборки программы. \
> Откройте консоль в данной папке и введите в консоли : `cmake <путь к файлу CMakeLists.txt> -DCMAKE_PREFIX_PATH= <путь к собранной библиотеке Protobuf>`. \
> Введите команду : `cmake --build .` . \
> После сборки в папке сборки появится исполняемый файл `transport_catalogue.exe`.

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

<details>
  <summary>Пример корректного файла make_base.json:</summary>

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

<details>
  <summary>Пример корректного файла process_requests.json:</summary>

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
