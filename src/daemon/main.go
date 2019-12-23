package main

import (
	"fmt"
	"bytes"
	"flag"

	"github.com/vmihailenco/msgpack"
)

func main() {
	println("Hello world")
	//searchHomeDir()
	agentMode := flag.Bool("agent", false, "is agent mode")
	homeDir := flag.String("dir", "", "directory for cremona.conf")
	flag.Parse()
	if *agentMode {
		println(*homeDir)
	}
	createDevice()

	// buff := new(bytes.Buffer)
	// buff.Write([]byte{0x91,0xdc, 0x0, 0x10, 0x58, 0xcc, 0xa9, 0x11, 0xcc, 0xc0, 0x1d, 0x54, 0x44, 0x51, 0xcc, 0x8a, 0x8, 0xcc, 0xe3, 0xcc, 0xf2, 0xcc, 0xdb, 0x2a, 0x47, 0xcc, 0xea})
	// dco := msgpack.NewDecoder(buff)
	// //nt := new([][]byte)
	// d, e:=dco.DecodeInterfaceLoose()
	// 	if e != nil {
	// 	panic(e)
	// }
	// //passInterface(&(d.([]interface{})[0]))
	// fmt.Printf("%#v\n", d.([]interface{})[0].([]int8)[0]);
}

func passInterface(v interface{}) {
    b, ok := v.(*[]byte)
    fmt.Println(ok)
    fmt.Println(b)
}

func createDevice() {
	mess := genCreateAccount("rnid")
	b, err := msgpack.Marshal(&mess)
	if err != nil {
		panic(err)
	}
	println(b)
	link, err2 := createNetlink()
	if err2 != nil {
		panic(err)
	}
	buf := new(bytes.Buffer)
	buf.Write(mess.Serialize())
	err = link.sendCreateConnection(mess)
	if err != nil {
		panic(err)
	}
	link.waitCommand()
}
