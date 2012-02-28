CREATE TABLE categories(
	id INTEGER primary key autoincrement,
	category_name VARCHAR2(100) NOT NULL
);
CREATE TABLE products(
	id INTEGER primary key autoincrement,
	product_name VARCHAR2(100) NOT NULL,
	category_id int DEFAULT 0,
	foreign key(category_id) references categories(id)
);
CREATE UNIQUE INDEX idx_products ON products(product_name, category_id);
CREATE TABLE shops(
	id INTEGER primary key autoincrement,
	shop_name VARCHAR2(100) UNIQUE NOT NULL
);
CREATE TABLE users(
	id INTEGER primary key autoincrement,
	login VARCHAR2(50) NOT NULL UNIQUE,
	password VARCHAR2(32)
);
CREATE TABLE expenses(
	id INTEGER primary key autoincrement,
	exp_date datetime DEFAULT CURRENT_TIMESTAMP,
	amount DOUBLE DEFAULT 1.0,
	product_id INTEGER NOT NULL,
	shop_id INTEGER DEFAULT 0,
	price DOUBLE NOT NULL,
	foreign key(shop_id) references shops(id),
	foreign key(product_id) references products(id)
);
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

INSERT INTO shops VALUES(0, '-');
INSERT INTO categories VALUES(0, '-');
