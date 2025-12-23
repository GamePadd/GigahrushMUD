# GigahrushMUD

## Структуры JSON:

Конфигурация сервера напрямую настраивается через JSON конфиги, все они разделены на несколько файлов для удобства работы:

### Items.json

[
	{
		"id" : 1,
		"name" : "Предмет1",
		"description" : "Описание1",
		"useDescription" : "Использование1",
		"type" : 0,
		"canSpawn" : true,
		"spawnChance" : 0.5
	},
	{
		"id" : 2,
		"name" : "Предмет2",
		"description" : "Описание2",
		"useDescription" : "Использование2",
		"type" : 0,
		"canSpawn" : true,
		"spawnChance" : 0.2
	},
	{
		"id" : 3,
		"name" : "Предмет3",
		"description" : "Описание3",
		"useDescription" : "Использование3",
		"type" : 1,
		"canSpawn" : false,
		"spawnChance" : 0.2
	}
]


### Enemies.json

[
	{
		"id" : 1,
		"name" : "Крутой враг",
		"description" : "Описание врага",
		"replics" : ["реплика1","реплика2"],
		"health" : 100,
		"attack" : 15,
		"spawnChance" : 0.5,
		"loot" : [1,2]
	},
	{
		"id" : 2,
		"name" : "Крутой враг 2",
		"description" : "Описание врага",
		"replics" : ["реплика1","реплика2"],
		"health" : 100,
		"attack" : 15,
		"spawnChance" : 0.5,
		"loot" : [1,2]
	}
]

### Rooms.json

[
	{
		"id" : 1,
		"name" : "Комната 1",
		"description" : "крутая комната",
		"itemDescs" : ["Описание1","Описание2"],
		"enemiesDescs" : ["Описание1","Описание2"],
		"isExit" : false,
		"spawnChance" : 0.9
	},
	{
		"id" : 2,
		"name" : "Комната 2",
		"description" : "хуйня комната",
		"itemDescs" : ["Описание1","Описание2"],
		"enemiesDescs" : ["Описание1","Описание2"],
		"isExit" : false,
		"spawnChance" : 0.5
	}
]

### Crafts.json

[
	{
		"id" : 3,
		"craft" : [1,2]
	}
]



### MainConfig.json

{
	"FloorCount" : 3,
	"FloorSizeX" : 10,
	"FloorSizeY" : 10
}

