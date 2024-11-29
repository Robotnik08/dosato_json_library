# Json library for the Dosato Programming Language

This library is used to turn parse JSON strings into Dosato objects and vice versa.

## Compilation

To compile the library, run the following command:

```bash
make
```

This will create a `json.dll` or `json.so` file that can be imported into Dosato programs.

## Usage

Theres a few examples in the `examples` directory that show how to use the library.

## Specifications

### `object json_parse(string json)`

This function takes a JSON string and returns a Dosato object.

```dosato
make object obj = json_parse("{\"key\": \"value\"}")
do sayln(obj->key) // value
```

### `string json_stringify(object obj, bool formatted = false)`

This function takes a Dosato object and returns a JSON string.

```dosato
make object obj = {
    key: "value"
}
do sayln(json_stringify(obj, false)) // {"key":"value"}
do sayln(json_stringify(obj, true)) 
/* 
{
    "key": "value"
}
*/
```