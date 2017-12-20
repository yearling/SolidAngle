# ResourceUsage
1. D3D11_USAGE_DEFAULT
	CPU 不可读写 ， GPU可读可写
2. D3D11_USAGE_IMMUTABLE
	CPU不可读写 ， GPU可读不可写
3. D3D11_USAGE_DYNAMIC
	CPU可写不可读， GPU可读不可写，使用map，从来从CPU==》GPU传数据
4. D3D11_USAGE_STAGING
	CPU可读可写， GPU可读可写， 用来从GPU==>CPU 返回数据 

| Resource Usage|Default|Dynamic |Immutable| Staging|
| ------------- |---------|---|---|-----|
| GPU-Read| yes | yes|yes|yes|
|GPU-Write| yes| NO  |  NO |yes|
|CPU-Read| NO|NO|NO|yes|
|CPU-Write|NO|yes|NO|yes|

##　Resource Bind Options
| Resource Can Be Bound As|Default|Dynamic |Immutable| Staging|
| ------------- |---------|---|---|-----|
| Input to a Stage| yes | yes|yes|NO|
|Output from a Stage| yes| NO  |  NO |NO|
