# HttpDynamicMessageInterface

Simple Http `Interface` to send custom messages to a MARTe application using the `HttpService`

## Configuration

This `Interface` has no configuration but must be encapsulated inside an `HttpObjectBrowser` and needs an `HttpService`
to work.

### Example

An example of configuration is the following:

```json
{
  "+HttpService": {
    "Class": "HttpService",
    "Port": 9094,
    "Timeout": 0,
    "WebRoot": "HttpObjectBrowser"
  },
  "+HttpObjectBrowser": {
    "Class": "HttpObjectBrowser",
    "Root": ".",
    "+DynMessageInterface": {
      "Class": "HttpDynamicMessageInterface"
    }
  }
}
```

## Functionality

When a valid `HTTP GET` request is recieved by this interface, it will create a Message and try send it.
The request have the follwoing arguments:
  - `destination`: name of the object to send the message to
  - `function`: name of the function to call
  - `mode`: message mode (`ExpectsReply` or `ExpectsIndirectReply`) 
  - `timeout`: optional reply timeout
  - any parameter needed by the function

The resulting requests will look like (using the example config):

```
http://IP:9094/DynMessageInterface?destination=GAM&function=SetValue&Value=10&mode=ExpectsReply
```

