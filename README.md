## PRC, an easy way to protect your class pointers.

PRC is a simple quick header-only utility I created to prevent pointer references being created and easily seen in disassembly by reverse engineers using bitwise encryption and randomization with key and offset system(s).
In NO WAY is PRC a way to deal with bad actors modifying class field data, or nor will it prevent master reversers from getting your pointers on demand. PRC is meant to combat static analysis.

## Usage

To add a pointer already created to PRC, use the Add() method like so:
```
GetPRCManager()->Add<DataType>(Data, Index); // Indexes are one use only.
```


To access a pointer already added to PRC, use the Get() method like so:
```
GetPRCManager()->Get<DataType>(Index); // Indexes are one use only.
```

## Example

Here's a screenshot of what the disassembly looks like when we create a Car pointer with a constructor. As you can see in the red circle, this is all somebody needs to have quick and easy access to our pointer, potentionally modifying data.
![Disassembly showing easy access to the pointer we created without PRC.](https://i.imgur.com/dGNNjNZ.png)

Here's the same binary disassembled using PRC to protect the original pointer created.
![Disassembly showing it would take much more work and effort to find this pointer on app startup from disassembly alone.](https://i.imgur.com/brDPV1n.png)
