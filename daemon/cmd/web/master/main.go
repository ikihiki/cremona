package main

import (
	"bytes"
	"encoding/json"
	"log"
	"net/http"
	"time"

	"github.com/labstack/echo/v4"
	"github.com/labstack/echo/v4/middleware"
	echoSwagger "github.com/swaggo/echo-swagger"

	_ "github.com/ikihiki/cremona/daemon/cmd/web/master/docs"

	"github.com/ikihiki/cremona/daemon/internal/config"
	"github.com/ikihiki/cremona/daemon/internal/daemon/master"
)

var a master.Master
var buf *bytes.Buffer

// @title Swagger Example API
// @version 1.0
// @description This is a sample swagger server.
// @license.name Apache 2.0
// @license.url http://www.apache.org/licenses/LICENSE-2.0.html
// @host localhost:32424
// @BasePath /api/v1
func main() {
	buf = new(bytes.Buffer)
	logger := log.New(buf, "[master] ", log.Ldate|log.Ltime)
	gconf := config.GlobalConfig{}
	uconfLoad := config.UserConfigLoad{}
	a = master.NewMaster(logger, gconf, uconfLoad)
	// Echo instance
	e := echo.New()

	// Middleware
	e.Use(middleware.Logger())
	e.Use(middleware.Recover())

	// Routes
	e.GET("/", redirectSwagger)
	e.GET("/api/v1/test", hello)
	e.GET("/api/v1/start", startServer)
	e.GET("/api/v1/log", getLog)
	e.GET("/api/v1/data", getData)
	e.GET("/swagger/*", echoSwagger.WrapHandler)

	// Start server
	e.Logger.Fatal(e.Start(":32424"))
}

// test
// @Summary test
// @Description get testdata
// @Success 200 {object} string
// @Router /test [get]
func hello(c echo.Context) error {
	return c.String(http.StatusOK, "Hello, World!")
}

func redirectSwagger(c echo.Context) error {
	return c.Redirect(http.StatusMovedPermanently, "/swagger/index.html")
}

type (
	Geolocation struct {
		Altitude  float64
		Latitude  float64
		longitude float64
	}
)

var (
	locations = []Geolocation{
		{-97, 37.819929, -122.478255},
		{1899, 39.096849, -120.032351},
		{2619, 37.865101, -119.538329},
		{42, 33.812092, -117.918974},
		{15, 37.77493, -122.419416},
	}
)

// start
// @Summary start server
// @Description get testdata
// @Produce application/x-json-stream
// @Success 200 {array} Geolocation
// @Router /start [get]
func startServer(c echo.Context) error {
	c.Response().Header().Set(echo.HeaderContentType, "application/x-json-stream")
	c.Response().WriteHeader(http.StatusOK)

	enc := json.NewEncoder(c.Response())
	for _, l := range locations {
		if err := enc.Encode(l); err != nil {
			return err
		}
		c.Response().Flush()
		time.Sleep(1 * time.Second)
	}
	return nil
}

// get log
// @Summary get log
// @Description get log bufferS
// @Success 200 {string} log
// @Router /log [get]
func getLog(c echo.Context) error {
	c.Response().Status = http.StatusOK
	_, err := c.Response().Write(buf.Bytes())
	if err != nil {
		return err
	}
	c.Response().Flush()
	return nil
}

// get data
// @Summary get data
// @Description get data
// @Success 200 {string} data
// @Router /data [get]
func getData(c echo.Context) error {
	s, err := master.DumpJosn(&a)
	if err != nil {
		return err
	}
	return c.String(http.StatusOK, s)
}
