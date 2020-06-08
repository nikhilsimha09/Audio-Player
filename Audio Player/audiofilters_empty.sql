-- phpMyAdmin SQL Dump
-- version 4.7.4
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Erstellungszeit: 17. Dez 2019 um 09:10
-- Server-Version: 10.1.30-MariaDB
-- PHP-Version: 7.0.27

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Datenbank: `audiofilters`
--

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `filtercoefficients`
--

CREATE TABLE `filtercoefficients` (
  `fcid` int(11) NOT NULL,
  `fid_fk` int(11) NOT NULL,
  `coefficient` float NOT NULL DEFAULT '0',
  `position` int(11) NOT NULL DEFAULT '0',
  `polynom` enum('NUMERATOR','DENOMINATOR') NOT NULL DEFAULT 'NUMERATOR'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `filters`
--

CREATE TABLE `filters` (
  `fid` int(11) NOT NULL,
  `ftid_fk` int(11) NOT NULL,
  `fs_Hz` int(11) DEFAULT NULL,
  `f_order` int(11) DEFAULT NULL,
  `delay_ms` int(11) DEFAULT NULL,
  `f_info` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `filtertypes`
--

CREATE TABLE `filtertypes` (
  `ftid` int(11) NOT NULL,
  `f_type` varchar(255) NOT NULL,
  `f_subtype` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Stellvertreter-Struktur des Views `filterview`
-- (Siehe unten für die tatsächliche Ansicht)
--
CREATE TABLE `filterview` (
`FilterID` int(11)
,`Type` varchar(255)
,`Subtype` varchar(255)
,`Filterorder` int(11)
,`Fs` int(11)
,`Delay` int(11)
,`Description` varchar(255)
);

-- --------------------------------------------------------

--
-- Struktur des Views `filterview`
--
DROP TABLE IF EXISTS `filterview`;

CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `filterview`  AS  select `filters`.`fid` AS `FilterID`,`filtertypes`.`f_type` AS `Type`,`filtertypes`.`f_subtype` AS `Subtype`,`filters`.`f_order` AS `Filterorder`,`filters`.`fs_Hz` AS `Fs`,`filters`.`delay_ms` AS `Delay`,`filters`.`f_info` AS `Description` from (`filtertypes` join `filters` on((`filters`.`ftid_fk` = `filtertypes`.`ftid`))) ;

--
-- Indizes der exportierten Tabellen
--

--
-- Indizes für die Tabelle `filtercoefficients`
--
ALTER TABLE `filtercoefficients`
  ADD PRIMARY KEY (`fcid`),
  ADD KEY `fk_filter` (`fid_fk`);

--
-- Indizes für die Tabelle `filters`
--
ALTER TABLE `filters`
  ADD PRIMARY KEY (`fid`),
  ADD UNIQUE KEY `uc_filters` (`ftid_fk`,`fs_Hz`,`f_order`,`delay_ms`);

--
-- Indizes für die Tabelle `filtertypes`
--
ALTER TABLE `filtertypes`
  ADD PRIMARY KEY (`ftid`),
  ADD UNIQUE KEY `uc_types` (`f_type`,`f_subtype`);

--
-- AUTO_INCREMENT für exportierte Tabellen
--

--
-- AUTO_INCREMENT für Tabelle `filtercoefficients`
--
ALTER TABLE `filtercoefficients`
  MODIFY `fcid` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=159;

--
-- AUTO_INCREMENT für Tabelle `filters`
--
ALTER TABLE `filters`
  MODIFY `fid` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=25;

--
-- AUTO_INCREMENT für Tabelle `filtertypes`
--
ALTER TABLE `filtertypes`
  MODIFY `ftid` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- Constraints der exportierten Tabellen
--

--
-- Constraints der Tabelle `filtercoefficients`
--
ALTER TABLE `filtercoefficients`
  ADD CONSTRAINT `fk_filter` FOREIGN KEY (`fid_fk`) REFERENCES `filters` (`fid`) ON DELETE CASCADE;

--
-- Constraints der Tabelle `filters`
--
ALTER TABLE `filters`
  ADD CONSTRAINT `fk_filtertypes` FOREIGN KEY (`ftid_fk`) REFERENCES `filtertypes` (`ftid`) ON DELETE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
