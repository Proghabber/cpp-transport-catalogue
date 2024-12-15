Транспортный справочник получает, хранит и возвращает информацию об остановках и маршрутах транспортных средств.
Может создавать картинки маршрутов в формате SVG. Находить кратчайший путь между остановками (на одном или разных маршрутах), показывая все пересадки, считая примерное время по заданным параметрам. Возвращать информацию об отдельной остановке — на каких она встречается маршрутах. Возвращать информацию о маршруте — длина, расстояние.
Запросы и ответы на них это данные в формате JSON их устройство показано далее. 
Для работы необходимо подключить файлы:
"json_reader.h"
"request_handler.h"
"transport_catalogue.h"

Основным элементом является объект  “handler::RequestHandler”  который и осуществляет обработку запросов. Для работы ему необходимы  объекты классов “readJson::JsonReader”, “catalogue::TransportCatalogue”, “render::SvgMaker” которые необходимо создать и передать в его конструктор. Ввод и возврат данных происходит через методы “handler::RequestHandler::ReadJson(std::istream &input)” и  "handler::RequestHandler::ReturnJson(std::ostream &output)" соответственно. Работа осуществляется примерно следующим образом:
readJson::JsonReader readders;
catalogue::TransportCatalogue transport; 
render::SvgMaker make; 
handler::RequestHandler handl(transport, make, readders);
handl.ReadJson(std::cin);
handl.ReturnJson(std::cout);

Устройство JSON на ввод.
 
Json на ввод состоит из секций:
base_requests  - данные на заполнение маршрутов и остановок,
stat_requests  –  запросы на получение данных из базы,
routing_settings  – настройки движения по маршруту ,
render_settings  - настройки построения карты
 
base_requests  – секция закладки маршрута  состоит списков маршрутов, представляет собой  либо маршрут :
"base_requests": [
          {
              "is_roundtrip": true,
              "name": "название маршрута",
              "stops": [
                  "название остановки ",
                  "название остановки ",
                  "название остановки ",
                  "название остановки "
              ],
              "type": "Bus"
          },
	{
              "latitude": 55.592028,
              "longitude": 37.653656,
              "name": "название остановки ",
              "road_distances": {
                  "название остановки ": 890
              },
              "type": "Stop"
          }
] 
type – тип маршрут или остановка (строки  "Bus"/ “stop”)
is_roundtrip – круговой маршрут или нет (true/false)
name – имя маршрута или остановки (строка)
stops - список остановок маршрута в воде строк (только для маршрута)
либо остановку (строки) :
type – тип маршрут или остановка (строки  "Bus"/ “stop”)
координаты остановки
latitude  –  широта (вещественное число)
longitude  –  долгота (вещественное число)
road_distances  –  связь с другими остановками задается словарем, где ключ имя остановки,  а значение  число расстояние между ними.

stat_requests  - секция  запросов  к базе, список из словарей 
"stat_requests": [
   	 { "id": 1, "type": "Map" },
   	 { "id": 2, "type": "Stop",  "name": " название остановки " },
   	 { "id": 3, "type": "Bus",  "name": "название маршрута " }
{"type": "Route",  "from": " название остановки ",  "to": " название остановки ",  "id": 4}
]
id - номер запроса (целое число, оно не должно повторятся) есть у всех запросов
type – тип запроса (строка) может быть:
Map – запрос на создание карты (визуализирует запрос),
Stop - запрос на получение информации о остановке,
Bus – запрос на получение информации о маршруте,
Route – запрос на получение кратчайшего пути между остановками
name  - имя остановки или маршрута (ключ в соответствующем словаре)
from – откуда строим маршрут (строка) 
to – куда строим маршрут (строка)

routing_settings  –  секция закладки  настроек маршрута,  словарь с двумя ключами:
"routing_settings": {
  	"bus_wait_time": 6,
 	"bus_velocity": 40
}
bus_wait_time — время ожидания автобуса на остановке, в минутах (Значение — целое число от 1 до 1000),
bus_velocity - скорость автобуса, в км/ч (Значение — вещественное число от 1 до 1000)

render_settings – секция настроек карты  (SVG изображение), словарь имеет ключи:
"render_settings": {
 "width": 200,
 "height": 200,
 "padding": 30,
 "stop_radius": 5,
 "line_width": 14,
 "bus_label_font_size": 20,
 "bus_label_offset": [7, 15],
 "stop_label_font_size": 20,
 "stop_label_offset": [7, -3],
 "underlayer_color": [255,255,255,0.85],
 "underlayer_width": 3,
 "color_palette": ["green", [255,160,0],"red"]
 }

width- ширина изображения (вещественное число),
height  – высота изображения (вещественное число),
padding – отступ (вещественное число),
stop_radius – размер точки остановки (вещественное число),
line_width  - ширина линий маршрута (вещественное число),
stop_label_font_size – размер текста надписей (целое число),
stop_label_offset  - список  из двух координат смещение текста  (вещественное число),
underlayer_color  - цвет rgb задается списком,
underlayer_width – ширена подложка текста (вещественное число),
color_palette –  палитра цветов для изображения.  Либо названия цвета, либо список,
bus_label_font_size  –  размер надписей маршрутов (целое число),
bus_label_offset  -  координаты смещения надписи маршрута задаются списком (вещественное число)

Пример json для ввода :  
{
      "base_requests": [
          {
              "is_roundtrip": true,
              "name": "297",
              "stops": [
                  "Biryulyovo Zapadnoye",  "Biryulyovo Tovarnaya", "Universam", "Biryulyovo Zapadnoye"
              ],
              "type": "Bus"
          },
          {
              "is_roundtrip": false,
              "name": "635",
              "stops": [
                  "Biryulyovo Tovarnaya", "Universam", "Prazhskaya"
              ],
              "type": "Bus"
          },
          {
              "latitude": 55.574371,
              "longitude": 37.6517,
              "name": "Biryulyovo Zapadnoye",
              "road_distances": {
                  "Biryulyovo Tovarnaya": 2600
              },
              "type": "Stop"
          },
          {
              "latitude": 55.587655,
              "longitude": 37.645687,
              "name": "Universam",
              "road_distances": {
                  "Biryulyovo Tovarnaya": 1380,
                  "Biryulyovo Zapadnoye": 2500,
                  "Prazhskaya": 4650
              },
              "type": "Stop"
          },
          {
              "latitude": 55.592028,
              "longitude": 37.653656,
              "name": "Biryulyovo Tovarnaya",
              "road_distances": {
                  "Universam": 890
              },
              "type": "Stop"
          },
          {
              "latitude": 55.611717,
              "longitude": 37.603938,
              "name": "Prazhskaya",
              "road_distances": {},
              "type": "Stop"
          }
      ],
      "render_settings": {
          "bus_label_font_size": 20,
          "bus_label_offset": [7, 15],
          "color_palette": ["green", [255,  160 0], "red" ],
          "height": 200,
          "line_width": 14,
          "padding": 30,
          "stop_label_font_size": 20,
          "stop_label_offset": [ 7, -3 ],
          "stop_radius": 5,
          "underlayer_color": [255, 255, 255, 0.85 ],
          "underlayer_width": 3,
          "width": 200
      },
      "routing_settings": {
          "bus_velocity": 40, "bus_wait_time": 6
      },
      "stat_requests": [
          {
              "id": 1, "name": "297", "type": "Bus"
          },
          {
              "id": 2,  "name": "635", "type": "Bus"
          },
          {
              "id": 3,  "name": "Universam", "type": "Stop"
          },
          {
             "id": 4,   "from": "Biryulyovo Zapadnoye",  "to": "Universam",  "type": "Route"
          },
          {
              "id": 5,  "from": "Biryulyovo Zapadnoye",  "to": "Prazhskaya", "type": "Route"
          }
	  {
              "id": 6, "type": "Map"
          }
      ]
  }

Устройство JSON на вывод.

Ответный json будет содержать список словарей для каждого запроса , общим для всех будет поле 
"request_id",  соответствующее  id запроса.  Также если запрос не удовлетворен  появляется ключ 
"error_message" содержащий текст  "not found".

На type запроса Map вернется словарь с колючем "map",  содержимое изображения  SVG формата.
На type запроса Route вернется словарь с ключами:
"items": [
              {
                  "stop_name": "Название остановки",
                  "time": 6,
                  "type": "Wait"
              },
              {
                  "bus": "Название маршрута",
                  "span_count": 2,
                  "time": 5.235,
                  "type": "Bus"
              }
          ],
          "request_id": 5,
          "total_time": 24.21
      }
"items"  - содержит список словарей элементов маршрута представленных словарями  где:
	"type" – поездка или ожидание ("Wait"/"Bus" строки),
"stop_name" – название остановки (строка ), если  "type":  "Wait",
"time"  -  время ожидания или поездки (вещественное число),
"bus"  - название  маршрута  (строка ),
"span_count"  - количество остановок пройденное на этом маршруте (целое число) , если  "type":  "Wait"
	"total_time" – общее время движение по составленному маршруту (целое число)
На type запроса Bus вернется словарь с ключами:
{
          "curvature": 1.30156,
          "request_id": 2,
          "route_length": 11570,
          "stop_count": 5,
          "unique_stop_count": 3
}

"curvature"  - кривизна маршрута (вещественное число),
"route_length”  – длинна маршрута (вещественное число),
 "stop_count" – общее число остановок на маршруте (целое число),
 "unique_stop_count" – количество уникальных остановок  (целое число)
На type запроса Stop вернется словарь с ключами:
{
          "buses": [
              "297",
              "635"
          ],
          "request_id": 3
}

"buses" -  список маршрутов где есть данная остановка. 


Пример json на вывод  :

[
    {
        "curvature": 1.42963,
        "request_id": 1,
        "route_length": 5990,
        "stop_count": 4,
        "unique_stop_count": 3
    },
    {
        "curvature": 1.30156,
        "request_id": 2,
        "route_length": 11570,
        "stop_count": 5,
        "unique_stop_count": 3
    },
    {
        "buses": ["297","635"],
        "request_id": 3
    },
    {
        "items": [
            {
                "stop_name": "Biryulyovo Zapadnoye",
                "time": 6,
                "type": "Wait"
            },
            {
                "bus": "297",
                "span_count": 2,
                "time": 5.235,
                "type": "Bus"
            }
        ],
        "request_id": 4,
        "total_time": 11.235
    },
    {
        "items": [
            {
                "stop_name": "Biryulyovo Zapadnoye",
                "time": 6,
                "type": "Wait"
            },
            {
                "bus": "297",
                "span_count": 1,
                "time": 3.9,
                "type": "Bus"
            },
            {
                "stop_name": "Biryulyovo Tovarnaya",
                "time": 6,
                "type": "Wait"
            },
            {
                "bus": "635",
                "span_count": 2,
                "time": 8.31,
                "type": "Bus"
            }
        ],
        "request_id": 5,
        "total_time": 24.21
    },
    {
        "map": "тело svg",
        "request_id": 6
    }
]


