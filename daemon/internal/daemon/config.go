package daemon

type GlobalConfigrator interface {
}

type UserConfigrator interface {
	GetServer() string
}

type UserConfigLoader interface {
	LoadConfig(baseDir string) (UserConfigrator, error)
}
