# Json library for the Dosato Programming Language

This library is used to turn parse JSON strings into Dosato objects and vice versa.


## Specifications

### `object json_parse(string json)`

This function takes a JSON string and returns a Dosato object.

```dosato
make object obj = json_parse("{\"key\": \"value\"}")
do sayln(obj->key) // value
```

### `string json_stringify(object obj)`

This function takes a Dosato object and returns a JSON string.

```dosato
make object obj = {
    key = "value"
}
do sayln(obj->key)
```