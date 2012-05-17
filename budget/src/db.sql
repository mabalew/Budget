CREATE TABLE categories(
	id integer primary key autoincrement,
	category_name varchar2(100) unique not null
);
CREATE UNIQUE INDEX categories_idx ON categories(id);

CREATE TABLE products(
	id integer primary key autoincrement,
	product_name varchar2(100),
	category_id int,
	foreign key(category_id) references categories(id)
);
CREATE UNIQUE INDEX products_idx on products(product_name, category_id);

CREATE TABLE shops(
	id integer primary key autoincrement,
	shop_name varchar2(100) unique
);
CREATE UNIQUE INDEX shops_idx ON shops(id);

CREATE TABLE users(
	id integer primary key autoincrement,
	login varchar2(50), password varchar2(32)
);
CREATE UNIQUE INDEX users_idx ON users(id);

CREATE TABLE expenses(
	id integer primary key autoincrement,
	exp_date datetime default CURRENT_TIMESTAMP,
	amount double default 1.0,
	product_id integer not null,
	shop_id integer default 0,
	price double,
	foreign key(shop_id) references shops(id),
	foreign key(product_id) references products(id)
);
CREATE UNIQUE INDEX expenses_idx ON expenses(id);

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
CREATE TABLE config(
	param varchar2(100) primary key,
	value varchar2(255) not null
);

CREATE TABLE tmp_expenses(
	id integer primary key autoincrement,
	category_id int,
	category varchar2(255),
	product_id int,
	product varchar2(255),
	count int default 1,
	amount double not null default 1,
	price double,
	shop_id int,
	shop varchar2(255),
	foreign key(category_id) references categories(id),
	foreign key(product_id)  references products(id),
	foreign key(shop_id)     references shops(id)
);
CREATE UNIQUE INDEX tmp_expenses_idx ON tmp_expenses(id);

INSERT INTO shops VALUES(0, '-');
INSERT INTO categories VALUES(0, '-');
INSERT INTO config VALUES('LOGGING_LEVEL', '2');
