* Accept-Charsets 
	Déf : Indique le jeu de caractères que le client est capable de comprendre.
	Comp. : - 
	Cat. : Request Header
	Ex : Accept-Charset: utf-8, iso-8859-1;q=0.5

* Accept-Language 
	Déf : Indique quelles sont les langues que le client est capable de comprendre, et quelle variante locale est préférée.
	Comp. : -
	Cat. : Request Header
	Accept-Language: fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5

* Allow
	Déf : L'entête Allow liste les méthodes supportées par une ressource.
	Comp. : -
	Cat. : Entity Header
	Allow: GET, POST, HEAD

* Authorization
	Déf : Contient les identifiants permettant l'authentification d'un utilisateur auprès d'un serveur.
	Comp. : -
	Cat. : Request Header
	Authorization: Basic YWxhZGRpbjpvcGVuc2VzYW1l

* Content-Language 
	Déf : Décrire quels langages sont destinés au public, de sorte 
	que cela permette à l'utilisateur de se différencier en fonction de la langue préférée des utilisateurs.
	Comp. : -
	Cat. : Entity header
	Content-Language: de-DE, en-CA

* Content-Length
	Déf : Indique la taille en octets (exprimée en base 10) 
	du corps de la réponse envoyée au client. 
	Comp. : -
	Cat. : Entity header
	Content-Length: 255

* Content-Location 
	Déf : Indicates an alternate location for the returned data. The principal use is to indicate 
	the URL of a resource transmitted as the result of content negotiation. 
	Comp. : -
	Cat. : Representation header
	Content-Location: /doc/index.json

* Content-Type
	Déf : Indicate the original media type of the resource (prior to any content encoding applied for sending).
	Comp. : -
	Cat. : Representation header
	Content-Type: multipart/form-data; boundary=something

* Date 
	Déf : Contient la date et l'heure d'origine du message.
	Comp. : -
	Cat. : General header
	Date: Wed, 21 Oct 2015 07:28:00 GMT

* Host
	Déf : Spécifie le nom de domaine du serveur (pour de l'hébergement virtuel), 
	et (optionnellement) le numéro du port TCP sur lequel le serveur écoute.
	Comp. : -
	Cat. : Request Header
	Host: developer.mozilla.org

* Last-Modified 
	Déf : Contient la date et l'heure à laquelle le serveur d'origine pense que la 
	ressource a été modifiée pour la dernière fois.
	Comp. : - 
	Cat. : Response Header
	Last-Modified: Wed, 21 Oct 2015 07:28:00 GMT

* Location
	Déf : Indique l'URL vers laquelle rediriger une page. Il a un sens seulement 
	lorsqu'il est servi avec une réponse d'état 3xx (redirection) ou 201 (créé).
	Comp. : -
	Cat. : Response Header
	Location: /index.html

* Referer
	Déf : Contient l'adresse de la page web précédente à partir de laquelle un 
	lien a été suivi pour demander la page courante.
	Comp. : -
	Cat. : Request Header
	Referer: https://developer.mozilla.org/fr/docs/Web/JavaScript

* Retry-After
	Déf : Indicates how long the user agent should wait before making a follow-up request.
	Comp. : -
	Cat. : Response Header
	Retry-After: Wed, 21 Oct 2015 07:28:00 GMT
	Retry-After: 120

* Server
	Déf : Describes the software used by the origin server that handled the request 
	— that is, the server that generated the response. 
	Comp. : -
	Cat. : Response Header
	Server: Apache/2.4.1 (Unix)

* Transfer-Encoding
	Déf : Specifies the form of encoding used to safely transfer the payload body to the user. 
	Comp. : -
	Cat. : General Header
	Transfer-Encoding: gzip, chunked
	Transfer-Encoding: compress

* User-Agent
	Déf : A characteristic string that lets servers and network peers identify 
	the application, operating system, vendor, and/or version of the requesting user agent.
	Comp. : -
	Cat. : Request Header
	User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0

* WWW-Authenticate
	Déf : Defines the HTTP authentication methods ("challenges") that might be 
	used to gain access to a specific resource.
	Comp. : -
	Cat. : Response Header
	WWW-Authenticate: Basic realm=<realm>, charset="UTF-8"
