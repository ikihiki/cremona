package main

import (
	"github.com/labstack/echo/v4"
	"github.com/labstack/echo/v4/middleware"
	"github.com/swaggo/echo-swagger"
	"net/http"

	_ "github.com/ikihiki/cremona/daemon/cmd/web/docs"
)
// @title Swagger Example API
// @version 1.0
// @description This is a sample swagger server.
// @license.name Apache 2.0
// @license.url http://www.apache.org/licenses/LICENSE-2.0.html
// @host localhost:32424
// @BasePath /api/v1
func main() {
	// Echo instance
	e := echo.New()

	// Middleware
	e.Use(middleware.Logger())
	e.Use(middleware.Recover())

	// Routes
	e.GET("/", redirect_swagger)
	e.GET("/api/v1/test", hello)
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

func redirect_swagger(c echo.Context) error  {
	return c.Redirect(http.StatusMovedPermanently, "/swagger/index.html")
}