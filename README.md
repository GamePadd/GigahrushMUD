# GigahrushMUD

## Структуры JSON:

Конфигурация сервера напрямую настраивается через JSON конфиги, все они разделены на несколько файлов для удобства работы:

### Items.json

[
	{
		"id" : 1,
		"name" : "Крутой предмет",
		"description" : "Ахуенное описание",
		"useDescription" : "Невьебенное использование",
		"type" : 0,
		"canSpawn" : true,
		"spawnChance" : 0.5
	},
	{
		"id" : 2,
		"name" : "Крутой предмет 2",
		"description" : "Ахуенное описание 2",
		"useDescription" : "Невьебенное использование 2",
		"type" : 0,
		"canSpawn" : true,
		"spawnChance" : 0.2
	},
	{
		"id" : 3,
		"name" : "Крутой предмет 3",
		"description" : "Ахуенное описание 3",
		"useDescription" : "Невьебенное использование 3",
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
		"description" : "Ахуенный враг",
		"replics" : ["реплика1","реплика2"],
		"health" : 100,
		"attack" : 15,
		"spawnChance" : 0.5,
		"loot" : [1,2]
	},
	{
		"id" : 2,
		"name" : "Крутой враг 2",
		"description" : "Ахуенный враг 2",
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
		"isExit" : false
		"spawnChance" : 0.5
	},
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

