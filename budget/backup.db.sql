PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE users(id integer primary key autoincrement, login varchar2(50), password varchar2(32));
INSERT INTO "users" VALUES(1,'mabalew','qqq');
INSERT INTO "users" VALUES(2,'user1','ppp');
INSERT INTO "users" VALUES(3,'user2','ppp');
INSERT INTO "users" VALUES(5,'qq','pp');
CREATE TABLE categories(id integer primary key autoincrement, category_name varchar2(100));
INSERT INTO "categories" VALUES(0,'-');
INSERT INTO "categories" VALUES(1,'Pieczywo');
INSERT INTO "categories" VALUES(2,'Nabial');
INSERT INTO "categories" VALUES(3,'Mieso');
INSERT INTO "categories" VALUES(4,'Napoje');
INSERT INTO "categories" VALUES(5,'Chemia');
INSERT INTO "categories" VALUES(6,'Inne');
INSERT INTO "categories" VALUES(7,'Uzywki');
INSERT INTO "categories" VALUES(8,'Roslinki');
CREATE TABLE products(id integer primary key autoincrement, product_name varchar2(100), category_id int);
INSERT INTO "products" VALUES(1,'Chleb',1);
INSERT INTO "products" VALUES(2,'Maslo',2);
INSERT INTO "products" VALUES(3,'Bulka',1);
INSERT INTO "products" VALUES(4,'Mleko',2);
INSERT INTO "products" VALUES(5,'Parowki',3);
INSERT INTO "products" VALUES(6,'Piwo',7);
INSERT INTO "products" VALUES(7,'Wino',7);
INSERT INTO "products" VALUES(8,'Wodka',7);
INSERT INTO "products" VALUES(9,'Jablka',8);
INSERT INTO "products" VALUES(10,'Gruszki',8);
INSERT INTO "products" VALUES(11,'Cytryny',8);
INSERT INTO "products" VALUES(12,'Kapusta',8);
INSERT INTO "products" VALUES(13,'Wloszczyzna',8);
INSERT INTO "products" VALUES(14,'Seler_naciowy',8);
INSERT INTO "products" VALUES(15,'Figi',8);
INSERT INTO "products" VALUES(16,'Kiwi',8);
INSERT INTO "products" VALUES(17,'Grapefruit',8);
INSERT INTO "products" VALUES(18,'Pomarancze',8);
INSERT INTO "products" VALUES(19,'Mandarynki',8);
INSERT INTO "products" VALUES(20,'Jogurt',2);
INSERT INTO "products" VALUES(21,'Serek_hom.',2);
INSERT INTO "products" VALUES(22,'Smietana',2);
INSERT INTO "products" VALUES(23,'Kefir',2);
CREATE TABLE shops(id integer primary key autoincrement, shop_name varchar2(100) unique);
INSERT INTO "shops" VALUES(0,'-');
INSERT INTO "shops" VALUES(1,'Lidl-Wyszkow');
INSERT INTO "shops" VALUES(2,'Biedronka-Wyszkow');
INSERT INTO "shops" VALUES(3,'Stokrotka-Wyszkow');
INSERT INTO "shops" VALUES(4,'Spozywczy-Mostowka');
INSERT INTO "shops" VALUES(5,'Spozywczy-Zazdrosc');
INSERT INTO "shops" VALUES(6,'Spozywczy-Niegow');
INSERT INTO "shops" VALUES(10,'Spozywczy-Zabrodzie');
CREATE TABLE expenses(id integer primary key autoincrement, exp_date datetime default CURRENT_TIMESTAMP, amount double default 1.0, product_id integer not null, shop_id integer default 0, price double, foreign key(shop_id) references shops(id), foreign key(product_id) references products(id));
INSERT INTO "expenses" VALUES(1,'2012-02-06 15:54:06',1.0,1,1,2.2);
INSERT INTO "expenses" VALUES(2,'2012-02-06 15:54:12',1.0,6,1,2.5);
INSERT INTO "expenses" VALUES(3,'2012-02-06 15:55:33',0.25,20,1,1.79);
INSERT INTO "expenses" VALUES(4,'2012-02-06 15:56:17',3.0,9,1,5.7);
DELETE FROM sqlite_sequence;
INSERT INTO "sqlite_sequence" VALUES('users',5);
INSERT INTO "sqlite_sequence" VALUES('categories',8);
INSERT INTO "sqlite_sequence" VALUES('products',23);
INSERT INTO "sqlite_sequence" VALUES('shops',10);
INSERT INTO "sqlite_sequence" VALUES('expenses',9);
CREATE VIEW v_expenses as
select e.id [exp id],
	e.exp_date,
	c.id [category_id],
	c.category_name,
	p.id [product_id],
	p.product_name,
	e.amount,
	e.price,
	(1 / e.amount) * e.price [unit_price],
	s.id [shop_id],
	s.shop_name
from
	expenses e, categories c, products p, shops s
where
	p.category_id = c.id and
	e.product_id = p.id and
	e.shop_id = s.id
group by e.id;
COMMIT;
