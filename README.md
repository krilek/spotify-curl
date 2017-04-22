# spotify-playlist-duplicates-remove
Simple c++ program that removes songs from specified playlist that also exist on user library.
Requirements: curl lib
Requires parameters:
 - Oauth (user-library-read, playlist-read-private, playlist-modify-private OR playlist-read-public, playlist-modify-public)
 Get it at https://developer.spotify.com/web-api/console/get-playlist-tracks/
 - user login (ex.: jmperezperez)
 - playlist ID (ex.: 3cEYpjA9oz9GiPac4AsH4n)
