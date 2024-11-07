# OnChangeLoggerDataSource

A `DataSource` which prints in the MARTe logger the current value of any
signal whenever one of the monitored signals changed.

## Configuration

This `DataSource` has no global parameters.

The `Signals` have the following parameters:


| Field          | Type  | Default | Description                                           |
|:---------------|------:|:-------:|:------------------------------------------------------|
| `Ignore`       | uint8 | `0`     | Do not check this signal for the logger trigger logic |

### Example

```json
{
  "LoggerDS" : {
    "Class": "OnChangeLoggerDataSource",
    "Signals": {
      "PLCCommand": {
        "Type": "uint8"
      }, 
      "PLCSDNCounter" : {
        "Type": "uint32",
        "Ignore": 1
      }
    }
  }
}
```

## Functionality

This `DataSource` use a custom made `OnChangeBrocker` that check if any of the monitored signals
changes and when it does it prints out the values using the MARTe logging system.

The output will be in the form of:

```
Log event: %LOG_EVENT_NUMBER
SIGNAL_NAME_A:%VALUE
SIGNAL_NAME_B:%VALUE
SIGNAL_NAME_C:%VALUE
```

The `%LOG_EVENT_NUMBER` increase each time a log is produced.

The signal name depends on the `GAM` that copies the value to the `OnChangeLoggerDataSource` and the 
ordrers depends on it as well.
