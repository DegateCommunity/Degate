# Downloads Security

Each download is created via Github actions, the task is automatic and performed by Github servers. The sources used to create these downloads are those available on Github. Therefore, there is no human intervention when creating them, this prevents any kind of security issue. 

When downloads are ready, they are uploaded from Github actions to a new Github release. Furthermore, for automatic updates via installers, they are uploaded to the [Degate's updates repository](https://github.com/DegateCommunity/DegateUpdates).

Also, each commit is signed with a PGP key and every update are handled and signed by one of the [maintainers](https://github.com/DegateCommunity/Degate/blob/develop/CONTRIBUTORS.md).

Every single part of Degate, from Github actions scripts to the website sources are available on Github. Moreover, Github allow a full history of every action (commit, Github actions trigger, updates...), which increases the security even more.

To know the full procedure, please refer to [this](https://github.com/DegateCommunity/DegateUpdates/blob/main/README.md#create-a-releasepre-release) link.

